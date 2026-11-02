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

#include "jsonnode.h"

#include "macros.h"
#include "stringutils.h"
#include "number.h"
#include "date.h"
#include "timestamp.h"
#include "interval.h"

/* --------------------------------------------------------------------------------------------- *
 * Local helpers: the JSON DOM expects/returns single-byte oratext, while OCILIB public strings
 * are otext (ANSI char or wide wchar_t). These helpers do a 1-to-1 unit conversion suitable for
 * the ASCII data used by the unit tests; multi-byte oratext would require real charset conversion.
 * --------------------------------------------------------------------------------------------- */

static oratext * JsonNodeNativeToOra
(
    OCI_Environment *env,
    const dbtext    *src,
    OCI_Size           src_count,
    OCI_Size          *ora_len
)
{
    oratext *dst = (oratext *) OcilibMemoryAlloc(env, OCI_IPC_VOID,
                                                 sizeof(oratext) * (src_count + 1), 1, FALSE);
    if (NULL != dst)
    {
        OCI_Size i;
        for (i = 0; i < src_count; i++)
        {
            dst[i] = (oratext) (unsigned char) src[i];
        }
        dst[src_count] = (oratext) 0;
        *ora_len       = src_count;
    }
    return dst;
}

static otext * JsonNodeOraToNative
(
    OCI_Environment *env,
    const oratext   *src,
    OCI_Size           src_count
)
{
    otext *dst = (otext *) OcilibMemoryAlloc(env, OCI_IPC_VOID,
                                             sizeof(otext) * (src_count + 1), 1, FALSE);
    if (NULL != dst)
    {
        OCI_Size i;
        for (i = 0; i < src_count; i++)
        {
            dst[i] = (otext) (unsigned char) src[i];
        }
        dst[src_count] = (otext) 0;
    }
    return dst;
}

static OCI_JsonType OcilibJsonNodeGetScalarType
(
    OCI_JsonNode *node
);

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeInitialize
(
    OCI_Json    *json,
    JsonDomNode *handle,
    OCI_JsonType type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

    OCI_JsonNode *node = NULL;

    CHECK_PTR(OCI_IPC_JSON, json);

    ALLOC_HANDLE(OCI_IPC_JSON_NODE, node);

    node->json   = json;
    node->handle = handle;
    node->type   = type;
    node->str    = NULL;

    if (NULL != json->jdoc && NULL != handle)
    {
        jznnodetype ntype = JsonDomGetNodeType(json->jdoc, (jznDomNode *) handle);

        switch (ntype)
        {
            case JZNDOM_OBJECT:
            {
                node->type = OCI_JNT_OBJECT;
                break;
            }
            case JZNDOM_ARRAY:
            {
                node->type = OCI_JNT_ARRAY;
                break;
            }
            case JZNDOM_SCALAR:
            {
                node->type = OcilibJsonNodeGetScalarType(node);
                break;
            }
            case JZNDOM_UNKNOWN:
            {
                break;
            }
            default:
            {
                break;
            }
        }
    }

    SET_RETVAL(node);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            FREE(node);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeFree
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

    CHECK_PTR(OCI_IPC_JSON_NODE, node);

    OcilibErrorResetSource(NULL, node);

    if (NULL != node->str)
    {
        OcilibMemoryFree(node->json->env, node->str);
        node->str = NULL;
    }

    FREE(node);

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetType
 * --------------------------------------------------------------------------------------------- */

static OCI_JsonType OcilibJsonNodeGetScalarType
(
    OCI_JsonNode *node
)
{
#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonType type = OCI_JNT_NULL;
    jznScalarVal sval;
    sword        res = 0;

    if (NULL == node || NULL == node->json || NULL == node->json->jdoc)
    {
        return OCI_JNT_NULL;
    }

    res = JsonDomGetScalarInfo(node->json->jdoc, (jznDomScalar*)node->handle, &sval);

    if (0 == res)
    {
        switch ((OCI_Int32) sval.type_jznScalarVal)
        {
            case JZNVAL_NULL:
                type = OCI_JNT_NULL;
                break;
            case JZNVAL_STRING:
                type = OCI_JNT_STRING;
                break;
            case JZNVAL_NUMBER:
            case JZNVAL_SINT:
            case JZNVAL_SLONG:
            case JZNVAL_UINT:
            case JZNVAL_ULONG:
            case JZNVAL_FLOAT:
            case JZNVAL_DOUBLE:
            case JZNVAL_CANONICAL_FLOAT:
            case JZNVAL_CANONICAL_DOUBLE:
            case JZNVAL_ORA_NUMBER:
            case JZNVAL_OCI_NUMBER:
                type = OCI_JNT_NUMBER;
                break;
            case JZNVAL_FALSE:
            case JZNVAL_TRUE:
            case JZNVAL_ORA_BOOLEAN:
                type = OCI_JNT_BOOLEAN;
                break;
            case JZNVAL_BINARY:
                type = OCI_JNT_BINARY;
                break;
            case JZNVAL_ORA_DATE:
            case JZNVAL_ANSI_DATE:
            case JZNVAL_OCI_DATE:
                type = OCI_JNT_DATE;
                break;
            case JZNVAL_ORA_TIMESTAMP:
            case JZNVAL_ORA_TIMESTAMPTZ:
            case JZNVAL_OCI_DATETIME:
                type = OCI_JNT_TIMESTAMP;
                break;
            case JZNVAL_ORA_YEARMONTH_DUR:
            case JZNVAL_ORA_DAYSECOND_DUR:
            case JZNVAL_OCI_INTERVAL:
                type = OCI_JNT_INTERVAL;
                break;
            default:
                type = OCI_JNT_NULL;
                break;
        }
    }

    return type;

#else

    OCI_NOT_USED(node);
    return OCI_JNT_NULL;

#endif
}

OCI_JsonType OcilibJsonNodeGetType
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonType, OCI_JNT_NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);

    if (OCI_JNT_NULL == node->type)
    {
        jznnodetype ntype = JsonDomGetNodeType(node->json->jdoc, (jznDomNode*)node->handle);

        switch ((OCI_Int32) ntype)
        {
            case JZNDOM_OBJECT:
                node->type = OCI_JNT_OBJECT;
                break;
            case JZNDOM_ARRAY:
                node->type = OCI_JNT_ARRAY;
                break;
            case JZNDOM_SCALAR:
                node->type = OcilibJsonNodeGetScalarType(node);
                break;
            default:
                node->type = OCI_JNT_NULL;
                break;
        }
    }

    SET_RETVAL(node->type);

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibJsonNodeGetString
(
    OCI_JsonNode *node,
    OCI_UInt32 *length
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    jznScalarVal sval;
    sword        res = 0;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_STRING) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    res = JsonDomGetScalarInfo(node->json->jdoc, (jznDomScalar*)node->handle, &sval);

    CHECK(res == 0);
    CHECK(sval.type_jznScalarVal == JZNVAL_STRING);

    if (NULL != node->str)
    {
        OcilibMemoryFree(node->json->env, node->str);
        node->str = NULL;
    }

    node->str = JsonNodeOraToNative(node->json->env,
                                    sval.val_jznScalarVal,
                                    (OCI_Size) sval.len_jznScalarVal);
    CHECK_NULL(node->str);

    if (NULL != length)
    {
        *length = (OCI_UInt32) sval.len_jznScalarVal;
    }

    SET_RETVAL((const otext *) node->str);

    CLEANUP_AND_EXIT_FUNC
    (
        ;
    );
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(length);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeIsNull
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeIsNull
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonType type = OCI_JNT_NULL;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);

    type = OcilibJsonNodeGetType(node);

    SET_RETVAL((OCI_Boolean)(OCI_JNT_NULL == type));

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetFieldCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibJsonNodeGetFieldCount
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    ub4 count = 0;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);

    count = JsonDomGetNumFields(node->json->jdoc, (jznDomObject*)node->handle);

    SET_RETVAL((OCI_UInt32) count);

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetFieldName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibJsonNodeGetFieldName
(
    OCI_JsonNode *node,
    OCI_UInt32  index
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    jznDomNameValuePair nvp;
    ub4                 count = 0;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    CHECK_MIN(index, 1);
    if (node->type != OCI_JNT_OBJECT) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    index--;
    if (index >= JsonDomGetNumFields(node->json->jdoc, (jznDomObject *)node->handle)) { THROW(OcilibExceptionOutOfBounds, (OCI_Int32) index); }

    memset(&nvp, 0, sizeof(nvp));

    count = JsonDomGetFields(node->json->jdoc, (jznDomObject*)node->handle,
                             (ub4)index, 1, &nvp);

    CHECK(count > 0);
    CHECK(nvp.name_jznDomNameValuePair.fnm_jznDomObjFieldName != NULL);

    if (NULL != node->str)
    {
        OcilibMemoryFree(node->json->env, node->str);
        node->str = NULL;
    }

    node->str = JsonNodeOraToNative(node->json->env,
                                    nvp.name_jznDomNameValuePair.fnm_jznDomObjFieldName,
                                    (OCI_Size) nvp.name_jznDomNameValuePair.len_jznDomObjFieldName);
    CHECK_NULL(node->str);

    SET_RETVAL((const otext *) node->str);

    CLEANUP_AND_EXIT_FUNC
    (
        ;
    );
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(index);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetFieldByName
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeGetFieldByName
(
    OCI_JsonNode *node,
    const otext  *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *field   = NULL;
    dbtext       *dbstr   = NULL;
    oratext      *orakey  = NULL;
    OCI_Size        ora_len = 0;
    OCI_Int32           dbsize  = -1;
    jznDomNode   *hnd     = NULL;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_STRING, name);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_OBJECT) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    dbstr = OcilibStringGetDBString(node->json->env, name, &dbsize);

    CHECK_NULL(dbstr);

    orakey = JsonNodeNativeToOra(node->json->env, dbstr,
                                 (OCI_Size) dbsize / sizeof(dbtext), &ora_len);
    CHECK_NULL(orakey);

    hnd = JsonDomGetFieldByName(node->json->jdoc, (jznDomObject*)node->handle,
                                orakey, (ub2) ora_len);

    CHECK_NULL(hnd);

    field = OcilibJsonNodeInitialize(node->json, hnd, OCI_JNT_NULL);

    CHECK_NULL(field);

    SET_RETVAL(field);

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != dbstr)
        {
            OcilibStringReleaseDBString(node->json->env, dbstr);
        }

        if (NULL != orakey)
        {
            OcilibMemoryFree(node->json->env, orakey);
        }

        if (FAILURE && NULL != field)
        {
            OcilibJsonNodeFree(field);
            field = NULL;
        }
    );
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(name);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetFieldByIndex
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeGetFieldByIndex
(
    OCI_JsonNode *node,
    OCI_UInt32  index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode       *field = NULL;
    jznDomNameValuePair nvp;
    ub4                 count = 0;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_OBJECT) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }
    if (index >= JsonDomGetNumFields(node->json->jdoc, (jznDomObject *)node->handle)) { THROW(OcilibExceptionOutOfBounds, (OCI_Int32) index); }

    memset(&nvp, 0, sizeof(nvp));

    count = JsonDomGetFields(node->json->jdoc, (jznDomObject*)node->handle,
                             (ub4)index, 1, &nvp);

    CHECK(count > 0);
    CHECK(nvp.node_jznDomNameValuePair != NULL);

    field = OcilibJsonNodeInitialize(node->json, nvp.node_jznDomNameValuePair, OCI_JNT_NULL);

    CHECK_NULL(field);

    SET_RETVAL(field);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != field)
        {
            OcilibJsonNodeFree(field);
            field = NULL;
        }
    );
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(index);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetItemCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibJsonNodeGetItemCount
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    ub4 count = 0;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);

    count = JsonDomGetArraySize(node->json->jdoc, (jznDomArray*)node->handle);

    SET_RETVAL((OCI_UInt32) count);

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetItem
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeGetItem
(
    OCI_JsonNode *node,
    OCI_UInt32  index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *item = NULL;
    jznDomNode   *hnd  = NULL;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_ARRAY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }
    if (index >= JsonDomGetArraySize(node->json->jdoc, (jznDomArray *)node->handle)) { THROW(OcilibExceptionOutOfBounds, (OCI_Int32) index); }

    hnd = JsonDomGetArrayElement(node->json->jdoc,
                                 (jznDomArray*)node->handle,
                                 (ub4)index);

    CHECK_NULL(hnd);

    item = OcilibJsonNodeInitialize(node->json, hnd, OCI_JNT_NULL);

    CHECK_NULL(item);

    SET_RETVAL(item);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != item)
        {
            OcilibJsonNodeFree(item);
            item = NULL;
        }
    );
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(index);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetNumber
 * --------------------------------------------------------------------------------------------- */

OCI_Number * OcilibJsonNodeGetNumber
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Number*, NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_Number  *value = NULL;
    jznScalarVal sval;
    JsonOCIVal   aux;
    sword        res   = 0;
    otext       *otstr = NULL;
    dbtext      *dbstr = NULL;
    dbtext      *dbfmt = NULL;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_NUMBER) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    memset(&sval, 0, sizeof(sval));
    memset(&aux,  0, sizeof(aux));

    res = JsonDomGetScalarInfoOci(node->json->jdoc, (jznDomScalar*)node->handle,
                                  &sval, &aux);

    CHECK(res == 0);
    CHECK((sval.type_jznScalarVal == JZNVAL_ORA_NUMBER) ||
          (sval.type_jznScalarVal == JZNVAL_OCI_NUMBER) ||
          (sval.type_jznScalarVal == JZNVAL_NUMBER) ||
          (sval.type_jznScalarVal == JZNVAL_SINT) ||
          (sval.type_jznScalarVal == JZNVAL_SLONG) ||
          (sval.type_jznScalarVal == JZNVAL_UINT) ||
          (sval.type_jznScalarVal == JZNVAL_ULONG));

    value = OcilibNumberCreate(node->json->env);

    CHECK_NULL(value);

    if (sval.type_jznScalarVal == JZNVAL_ORA_NUMBER ||
        sval.type_jznScalarVal == JZNVAL_OCI_NUMBER)
    {
        memset(value->handle, 0, (OCI_Size) OCI_NUMBER_SIZE);

        if (sval.oranumlen_jznScalarVal == (ub4) OCI_NUMBER_SIZE)
        {
            memcpy(value->handle, sval.oranumval_jznScalarVal, (OCI_Size) sval.oranumlen_jznScalarVal);
        }
        else
        {
            ((ub1 *) value->handle)[0] = (ub1) sval.oranumlen_jznScalarVal;
            memcpy(((ub1 *) value->handle) + 1, sval.oranumval_jznScalarVal,
                   (OCI_Size) sval.oranumlen_jznScalarVal);
        }
    }
    else
    {
        const otext *fmt     = OcilibEnvironmentGetFormat(node->json->env, OCI_FMT_NUMERIC);
        OCI_Int32          dbsize  = -1;
        OCI_Int32          fmtsize = -1;

        CHECK_NULL(fmt);

        /* the DOM keeps the scalar text in oratext form; OCINumberFromText
           expects both the value and the format in the environment charset */

        otstr = JsonNodeOraToNative(node->json->env, sval.val_jznScalarVal,
                                    (OCI_Size) sval.len_jznScalarVal);

        CHECK_NULL(otstr);

        dbstr = OcilibStringGetDBString(node->json->env, otstr, &dbsize);
        dbfmt = OcilibStringGetDBString(node->json->env, fmt, &fmtsize);

        CHECK_NULL(dbstr);
        CHECK_NULL(dbfmt);

        CHECK_OCI
        (
            node->json->env->err,
            OCINumberFromText,
            node->json->env->err, (oratext *) dbstr, (ub4) dbsize,
            (oratext *) dbfmt, (ub4) fmtsize, (oratext *) NULL, (ub4) 0, value->handle
        );
    }

    SET_RETVAL(value);

    CLEANUP_AND_EXIT_FUNC
    (
        FREE(otstr);
        OcilibStringReleaseDBString(node->json->env, dbstr);
        OcilibStringReleaseDBString(node->json->env, dbfmt);

        if (FAILURE && NULL != value)
        {
            OcilibNumberFree(value);
            value = NULL;
        }
    );
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetBoolean
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeGetBoolean
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_Boolean  value = FALSE;
    jznScalarVal sval;
    sword        res = 0;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_BOOLEAN) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    res = JsonDomGetScalarInfo(node->json->jdoc, (jznDomScalar*)node->handle, &sval);

    CHECK(res == 0);
    CHECK((sval.type_jznScalarVal == JZNVAL_TRUE) ||
          (sval.type_jznScalarVal == JZNVAL_FALSE) ||
          (sval.type_jznScalarVal == JZNVAL_ORA_BOOLEAN));

    if (sval.type_jznScalarVal == JZNVAL_TRUE)
    {
        value = TRUE;
    }
    else if (sval.type_jznScalarVal == JZNVAL_FALSE)
    {
        value = FALSE;
    }
    else if (sval.type_jznScalarVal == JZNVAL_ORA_BOOLEAN)
    {
        value = (*sval.oranumval_jznScalarVal) ? TRUE : FALSE;
    }

    SET_RETVAL(value);

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetBinary
 * --------------------------------------------------------------------------------------------- */

const void * OcilibJsonNodeGetBinary
(
    OCI_JsonNode *node,
    OCI_UInt32 *length
)
{
    ENTER_FUNC
    (
        /* returns */ const void*, NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    const void  *value = NULL;
    jznScalarVal sval;
    sword        res = 0;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_BINARY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    res = JsonDomGetScalarInfo(node->json->jdoc, (jznDomScalar*)node->handle, &sval);

    CHECK(res == 0);
    CHECK(sval.type_jznScalarVal == JZNVAL_BINARY);

    value = (const void *) sval.binval_jznScalarVal;

    if (NULL != length)
    {
        *length = (OCI_UInt32) sval.binlen_jznScalarVal;
    }

    SET_RETVAL(value);

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(length);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OcilibJsonNodeGetDate
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Date*, NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_Date    *value = NULL;
    jznScalarVal sval;
    JsonOCIVal   aux;
    sword        res = 0;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_DATE) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    memset(&sval, 0, sizeof(sval));
    memset(&aux,  0, sizeof(aux));

    res = JsonDomGetScalarInfoOci(node->json->jdoc, (jznDomScalar*)node->handle,
                                  &sval, &aux);

    CHECK(res == 0);
    CHECK((sval.type_jznScalarVal == JZNVAL_ORA_DATE) ||
          (sval.type_jznScalarVal == JZNVAL_OCI_DATE));

    value = OcilibDateCreate(node->json->con->env);

    CHECK_NULL(value);

    value->handle->OCIDateYYYY           = (sb2) aux.dt_JsonOCIVal.year_JsonDateTime;
    value->handle->OCIDateMM             = (ub1) aux.dt_JsonOCIVal.month_JsonDateTime;
    value->handle->OCIDateDD             = (ub1) aux.dt_JsonOCIVal.day_JsonDateTime;
    value->handle->OCIDateTime.OCITimeHH = (ub1) aux.dt_JsonOCIVal.hour_JsonDateTime;
    value->handle->OCIDateTime.OCITimeMI = (ub1) aux.dt_JsonOCIVal.minute_JsonDateTime;
    value->handle->OCIDateTime.OCITimeSS = (ub1) aux.dt_JsonOCIVal.second_JsonDateTime;

    SET_RETVAL(value);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != value)
        {
            OcilibDateFree(value);
            value = NULL;
        }
    );
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OcilibJsonNodeGetTimestamp
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Timestamp*, NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_Timestamp    *value = NULL;
    jznScalarVal      sval;
    JsonOCIVal        aux;
    sword             res   = 0;
    OCI_TimestampType ttype = OCI_TIMESTAMP;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_TIMESTAMP) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    memset(&sval, 0, sizeof(sval));
    memset(&aux,  0, sizeof(aux));

    res = JsonDomGetScalarInfoOci(node->json->jdoc, (jznDomScalar*)node->handle,
                                  &sval, &aux);

    CHECK(res == 0);
    CHECK((sval.type_jznScalarVal == JZNVAL_ORA_TIMESTAMP) ||
          (sval.type_jznScalarVal == JZNVAL_ORA_TIMESTAMPTZ) ||
          (sval.type_jznScalarVal == JZNVAL_OCI_DATETIME));

    if ((sval.type_jznScalarVal == JZNVAL_ORA_TIMESTAMPTZ) ||
        ((sval.type_jznScalarVal == JZNVAL_OCI_DATETIME) &&
         ((aux.dt_JsonOCIVal.tzHourOffset_JsonDateTime != 0) ||
          (aux.dt_JsonOCIVal.tzMinuteOffset_JsonDateTime != 0))))
    {
        ttype = OCI_TIMESTAMP_TZ;
    }

    value = OcilibTimestampCreate(node->json->con->env, ttype);

    CHECK_NULL(value);

    {
        signed char tz_hour   = (signed char) aux.dt_JsonOCIVal.tzHourOffset_JsonDateTime;
        signed char tz_minute = (signed char) aux.dt_JsonOCIVal.tzMinuteOffset_JsonDateTime;
        otext       tz[8]     = { 0 };
        OraText    *tzp       = (OraText *)NULL;
        OCI_Size      tz_len    = 0;

        if (OCI_TIMESTAMP_TZ == ttype)
        {
            /* timezone text must be in the OCI environment character set */

            OCI_Int32 hour   = (tz_hour   < 0) ? -tz_hour   : tz_hour;
            OCI_Int32 minute = (tz_minute < 0) ? -tz_minute : tz_minute;

            tz[0] = (tz_hour < 0) ? OTEXT('-') : OTEXT('+');
            tz[1] = (otext)(OTEXT('0') + hour / 10);
            tz[2] = (otext)(OTEXT('0') + hour % 10);
            tz[3] = OTEXT(':');
            tz[4] = (otext)(OTEXT('0') + minute / 10);
            tz[5] = (otext)(OTEXT('0') + minute % 10);
            tz[6] = 0;

            tz_len = otextsize(tz);
            tzp    = (OraText *)tz;
        }

        CHECK_OCI
        (
            node->json->con->env->err,
            OCIDateTimeConstruct,
            node->json->con->env->handle,
            node->json->con->env->err,
            value->handle,
            (sb2) aux.dt_JsonOCIVal.year_JsonDateTime,
            (ub1) aux.dt_JsonOCIVal.month_JsonDateTime,
            (ub1) aux.dt_JsonOCIVal.day_JsonDateTime,
            (ub1) aux.dt_JsonOCIVal.hour_JsonDateTime,
            (ub1) aux.dt_JsonOCIVal.minute_JsonDateTime,
            (ub1) aux.dt_JsonOCIVal.second_JsonDateTime,
            (ub4) aux.dt_JsonOCIVal.fsecond_JsonDateTime,
            tzp,
            tz_len
        );
    }

    SET_RETVAL(value);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != value)
        {
            OcilibTimestampFree(value);
            value = NULL;
        }
    );
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OcilibJsonNodeGetInterval
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Interval*, NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_Interval    *value = NULL;
    jznScalarVal     sval;
    JsonOCIVal       aux;
    sword            res   = 0;
    OCI_IntervalType itype = OCI_INTERVAL_YM;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_INTERVAL) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    memset(&sval, 0, sizeof(sval));
    memset(&aux,  0, sizeof(aux));

    res = JsonDomGetScalarInfoOci(node->json->jdoc, (jznDomScalar*)node->handle,
                                  &sval, &aux);

    CHECK(res == 0);
    CHECK((sval.type_jznScalarVal == JZNVAL_ORA_YEARMONTH_DUR) ||
          (sval.type_jznScalarVal == JZNVAL_ORA_DAYSECOND_DUR) ||
          (sval.type_jznScalarVal == JZNVAL_OCI_INTERVAL));

    if ((sval.type_jznScalarVal == JZNVAL_ORA_DAYSECOND_DUR) ||
        ((sval.type_jznScalarVal == JZNVAL_OCI_INTERVAL) &&
         (sval.oranumlen_jznScalarVal >= 11)))
    {
        itype = OCI_INTERVAL_DS;
    }

    value = OcilibIntervalCreate(node->json->con->env, itype);

    CHECK_NULL(value);

    if (OCI_INTERVAL_YM == itype)
    {
        CHECK_OCI
        (
            node->json->con->env->err,
            OCIIntervalSetYearMonth,
            node->json->con->env->handle,
            node->json->con->env->err,
            (sb4) aux.yrInv_JsonOCIVal.years_JsonYearInterval,
            (sb4) aux.yrInv_JsonOCIVal.months_JsonYearInterval,
            value->handle
        );
    }
    else
    {
        CHECK_OCI
        (
            node->json->con->env->err,
            OCIIntervalSetDaySecond,
            node->json->con->env->handle,
            node->json->con->env->err,
            (sb4) aux.dayInv_JsonOCIVal.days_JsonDayInterval,
            (sb4) aux.dayInv_JsonOCIVal.hours_JsonDayInterval,
            (sb4) aux.dayInv_JsonOCIVal.minutes_JsonDayInterval,
            (sb4) aux.dayInv_JsonOCIVal.seconds_JsonDayInterval,
            (sb4) aux.dayInv_JsonOCIVal.fseconds_JsonDayInterval,
            value->handle
        );
    }

    SET_RETVAL(value);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != value)
        {
            OcilibIntervalFree(value);
            value = NULL;
        }
    );
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreateObject
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreateObject
(
    OCI_Json *json
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *node = NULL;
    jznDomNode   *hnd  = NULL;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(json->jdoc);

    hnd = (jznDomNode *) JsonDomNewObject(json->jdoc, 0);

    CHECK_NULL(hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_OBJECT);

    CHECK_NULL(node);

    SET_RETVAL(node);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
#else
    OCI_NOT_USED(json);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreateArray
(
    OCI_Json *json
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *node = NULL;
    jznDomNode   *hnd  = NULL;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(json->jdoc);

    hnd = (jznDomNode *) JsonDomNewArray(json->jdoc, 0);

    CHECK_NULL(hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_ARRAY);

    CHECK_NULL(node);

    SET_RETVAL(node);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
#else
    OCI_NOT_USED(json);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreateNull
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreateNull
(
    OCI_Json *json
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *node = NULL;
    jznDomNode   *hnd  = NULL;
    jznScalarVal  sval;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(json->jdoc);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = JZNVAL_NULL;

    hnd = (jznDomNode *) JsonDomNewScalar(json->jdoc, &sval);

    CHECK_NULL(hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_NULL);

    CHECK_NULL(node);

    SET_RETVAL(node);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
#else
    OCI_NOT_USED(json);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreateString
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreateString
(
    OCI_Json    *json,
    const otext *value,
    OCI_UInt32 length
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *node = NULL;
    jznDomNode   *hnd  = NULL;
    jznScalarVal  sval;
    dbtext       *dbstr   = NULL;
    oratext      *orastr  = NULL;
    OCI_Size        ora_len = 0;
    OCI_Int32           dbsize  = (length == 0 ? -1 : (OCI_Int32)(length * sizeof(otext)));

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_STRING, value);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(json->jdoc);

    dbstr = OcilibStringGetDBString(json->env, value, &dbsize);

    CHECK_NULL(dbstr);

    orastr = JsonNodeNativeToOra(json->env, dbstr,
                                 (OCI_Size) dbsize / sizeof(dbtext), &ora_len);
    CHECK_NULL(orastr);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = JZNVAL_STRING;
    sval.val_jznScalarVal  = orastr;
    sval.len_jznScalarVal  = (ub4) ora_len;

    hnd = (jznDomNode *) JsonDomNewScalar(json->jdoc, &sval);

    CHECK_NULL(hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_STRING);

    CHECK_NULL(node);

    SET_RETVAL(node);

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != dbstr)
        {
            OcilibStringReleaseDBString(json->env, dbstr);
        }

        if (NULL != orastr)
        {
            OcilibMemoryFree(json->env, orastr);
        }

        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    OCI_NOT_USED(length);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreateNumber
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreateNumber
(
    OCI_Json   *json,
    OCI_Number *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *node = NULL;
    jznDomNode   *hnd  = NULL;
    jznScalarVal  sval;
    ub1          *numbuf = NULL;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_NUMBER, value);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(json->jdoc);

    numbuf = (ub1 *) malloc(OCI_NUMBER_SIZE);

    CHECK_NULL(numbuf);

    memcpy(numbuf, value->handle, (OCI_Size) OCI_NUMBER_SIZE);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal      = JZNVAL_ORA_NUMBER;
    sval.oranumval_jznScalarVal = numbuf;
    sval.oranumlen_jznScalarVal = (ub4) OCI_NUMBER_SIZE;

    hnd = (jznDomNode *) JsonDomNewScalar(json->jdoc, &sval);

    CHECK_NULL(hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_NUMBER);

    CHECK_NULL(node);

    SET_RETVAL(node);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            free(numbuf);
            numbuf = NULL;
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreateBoolean
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreateBoolean
(
    OCI_Json   *json,
    OCI_Boolean value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *node = NULL;
    jznDomNode   *hnd  = NULL;
    jznScalarVal  sval;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(json->jdoc);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = (0 != value) ? JZNVAL_TRUE : JZNVAL_FALSE;

    hnd = (jznDomNode *) JsonDomNewScalar(json->jdoc, &sval);

    CHECK_NULL(hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_BOOLEAN);

    CHECK_NULL(node);

    SET_RETVAL(node);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreateBinary
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreateBinary
(
    OCI_Json    *json,
    const void  *value,
    OCI_UInt32 length
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *node = NULL;
    jznDomNode   *hnd  = NULL;
    jznScalarVal  sval;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK(0 == length || NULL != value);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(json->jdoc);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal   = JZNVAL_BINARY;
    sval.binval_jznScalarVal = (ub1 *) value;
    sval.binlen_jznScalarVal = (ub4) length;

    hnd = (jznDomNode *) JsonDomNewScalar(json->jdoc, &sval);

    CHECK_NULL(hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_BINARY);

    CHECK_NULL(node);

    SET_RETVAL(node);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    OCI_NOT_USED(length);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetString
(
    OCI_JsonNode *node,
    const otext  *value,
    OCI_UInt32  length
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    jznScalarVal sval;
    jznDomNode  *hnd     = NULL;
    dbtext      *dbstr   = NULL;
    oratext     *orastr  = NULL;
    OCI_Size       ora_len = 0;
    OCI_Int32          dbsize  = (length == 0 ? -1 : (OCI_Int32)(length * sizeof(otext)));

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_STRING, value);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_STRING) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    dbstr = OcilibStringGetDBString(node->json->env, value, &dbsize);

    CHECK_NULL(dbstr);

    orastr = JsonNodeNativeToOra(node->json->env, dbstr,
                                 (OCI_Size) dbsize / sizeof(dbtext), &ora_len);
    CHECK_NULL(orastr);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = JZNVAL_STRING;
    sval.val_jznScalarVal  = orastr;
    sval.len_jznScalarVal  = (ub4) ora_len;

    hnd = (jznDomNode *) JsonDomNewScalar(node->json->jdoc, &sval);
    CHECK_NULL(hnd);

    node->handle = hnd;

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != dbstr)
        {
            OcilibStringReleaseDBString(node->json->env, dbstr);
        }

        if (NULL != orastr)
        {
            OcilibMemoryFree(node->json->env, orastr);
        }
    );
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    OCI_NOT_USED(length);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetNumber
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetNumber
(
    OCI_JsonNode *node,
    OCI_Number   *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    jznScalarVal sval;
    jznDomNode  *hnd    = NULL;
    ub1         *numbuf = NULL;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_NUMBER, value);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_NUMBER) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    numbuf = (ub1 *) malloc((OCI_Size) OCI_NUMBER_SIZE);

    CHECK_NULL(numbuf);

    memcpy(numbuf, value->handle, (OCI_Size) OCI_NUMBER_SIZE);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal      = JZNVAL_ORA_NUMBER;
    sval.oranumval_jznScalarVal = numbuf;
    sval.oranumlen_jznScalarVal = (ub4) OCI_NUMBER_SIZE;

    hnd = (jznDomNode *) JsonDomNewScalar(node->json->jdoc, &sval);
    CHECK_NULL(hnd);

    node->handle = hnd;

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            free(numbuf);
            numbuf = NULL;
        }
    );
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetBoolean
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetBoolean
(
    OCI_JsonNode *node,
    OCI_Boolean   value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    jznScalarVal sval;
    jznDomNode  *hnd = NULL;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_BOOLEAN) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = (0 != value) ? JZNVAL_TRUE : JZNVAL_FALSE;

    hnd = (jznDomNode *) JsonDomNewScalar(node->json->jdoc, &sval);
    CHECK_NULL(hnd);

    node->handle = hnd;

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetBinary
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetBinary
(
    OCI_JsonNode *node,
    const void   *value,
    OCI_UInt32  length
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    jznScalarVal sval;
    jznDomNode  *hnd = NULL;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_VOID, value);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_BINARY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal   = JZNVAL_BINARY;
    sval.binval_jznScalarVal = (ub1 *) value;
    sval.binlen_jznScalarVal = (ub4) length;

    hnd = (jznDomNode *) JsonDomNewScalar(node->json->jdoc, &sval);
    CHECK_NULL(hnd);

    node->handle = hnd;

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    OCI_NOT_USED(length);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetNull
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetNull
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    jznScalarVal sval;
    jznDomNode  *hnd = NULL;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = JZNVAL_NULL;

    hnd = (jznDomNode *) JsonDomNewScalar(node->json->jdoc, &sval);
    CHECK_NULL(hnd);

    node->handle = hnd;

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetField
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetField
(
    OCI_JsonNode *obj,
    const otext  *name,
    OCI_JsonNode *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, obj
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    dbtext  *dbstr   = NULL;
    oratext *orakey  = NULL;
    OCI_Size   ora_len = 0;
    OCI_Int32      dbsize  = -1;

    CHECK_PTR(OCI_IPC_JSON_NODE, obj);
    CHECK_PTR(OCI_IPC_STRING, name);
    CHECK_PTR(OCI_IPC_JSON_NODE, value);
    CHECK_PTR(OCI_IPC_JSON, obj->json);
    CHECK_PTR(OCI_IPC_CONNECTION, obj->json->con);
    CHECK_FEATURE(obj->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(obj->json->jdoc);
    if (obj->type != OCI_JNT_OBJECT) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }
    CHECK_COMPAT(value->json == obj->json);

    dbstr = OcilibStringGetDBString(obj->json->env, name, &dbsize);

    CHECK_NULL(dbstr);

    orakey = JsonNodeNativeToOra(obj->json->env, dbstr,
                                 (OCI_Size) dbsize / sizeof(dbtext), &ora_len);
    CHECK_NULL(orakey);

    CHECK(JsonDomPutFieldValue(obj->json->jdoc, (jznDomObject *)obj->handle,
                               orakey, (ub2) ora_len, value->handle) == 0);

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != dbstr)
        {
            OcilibStringReleaseDBString(obj->json->env, dbstr);
        }

        if (NULL != orakey)
        {
            OcilibMemoryFree(obj->json->env, orakey);
        }
    );
#else
    OCI_NOT_USED(obj);
    OCI_NOT_USED(name);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetItem
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetItem
(
    OCI_JsonNode *arr,
    OCI_UInt32  index,
    OCI_JsonNode *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, arr
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, arr);
    CHECK_PTR(OCI_IPC_JSON_NODE, value);
    CHECK_PTR(OCI_IPC_JSON, arr->json);
    CHECK_PTR(OCI_IPC_CONNECTION, arr->json->con);
    CHECK_FEATURE(arr->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(arr->json->jdoc);
    if (arr->type != OCI_JNT_ARRAY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }
    CHECK_COMPAT(value->json == arr->json);

    if (index >= JsonDomGetArraySize(arr->json->jdoc, (jznDomArray *)arr->handle)) { THROW(OcilibExceptionOutOfBounds, (OCI_Int32) index); }

    CHECK(JsonDomPutItem(arr->json->jdoc, (jznDomArray *)arr->handle,
                         (ub4)(index + 1), value->handle) != 0);

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(arr);
    OCI_NOT_USED(index);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeAppendItem
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeAppendItem
(
    OCI_JsonNode *arr,
    OCI_JsonNode *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, arr
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, arr);
    CHECK_PTR(OCI_IPC_JSON_NODE, value);
    CHECK_PTR(OCI_IPC_JSON, arr->json);
    CHECK_PTR(OCI_IPC_CONNECTION, arr->json->con);
    CHECK_FEATURE(arr->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(arr->json->jdoc);
    if (arr->type != OCI_JNT_ARRAY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }
    CHECK_COMPAT(value->json == arr->json);

    CHECK(JsonDomAppendItem(arr->json->jdoc, (jznDomArray *)arr->handle, value->handle) != 0);

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(arr);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreateDate
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreateDate
(
    OCI_Json *json,
    OCI_Date *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *node = NULL;
    jznDomNode   *hnd  = NULL;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_DATE, value);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(json->jdoc);

    hnd = (jznDomNode *) JsonDomNewScalarVal(json->jdoc, JZNVAL_OCI_DATE, value->handle);

    CHECK_NULL(hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_DATE);

    CHECK_NULL(node);

    SET_RETVAL(node);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreateTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreateTimestamp
(
    OCI_Json      *json,
    OCI_Timestamp *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *node = NULL;
    jznDomNode   *hnd  = NULL;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_TIMESTAMP, value);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(json->jdoc);

    hnd = (jznDomNode *) JsonDomNewScalarVal(json->jdoc, JZNVAL_OCI_DATETIME, value->handle);

    CHECK_NULL(hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_TIMESTAMP);

    CHECK_NULL(node);

    SET_RETVAL(node);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreateInterval
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreateInterval
(
    OCI_Json     *json,
    OCI_Interval *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *node = NULL;
    jznDomNode   *hnd  = NULL;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_INTERVAL, value);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(json->jdoc);

    hnd = (jznDomNode *) JsonDomNewScalarVal(json->jdoc, JZNVAL_OCI_INTERVAL, value->handle);

    CHECK_NULL(hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_INTERVAL);

    CHECK_NULL(node);

    SET_RETVAL(node);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetDate
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibJsonNodeSetTemporal
(
    OCI_JsonNode *node,
    jznvaltype    vtype,
    void         *hnd
)
{
#if OCI_VERSION_COMPILE >= OCI_21_1

    jznDomScalar *tmp = NULL;

    if (NULL == node || NULL == node->json || NULL == node->json->jdoc || NULL == hnd)
    {
        return FALSE;
    }

    tmp = JsonDomNewScalarVal(node->json->jdoc, vtype, hnd);

    if (NULL == tmp)
    {
        return FALSE;
    }

    node->handle = (jznDomNode *) tmp;

    return TRUE;

#else

    OCI_NOT_USED(node);
    OCI_NOT_USED(vtype);
    OCI_NOT_USED(hnd);
    return FALSE;

#endif
}

OCI_Boolean OcilibJsonNodeSetDate
(
    OCI_JsonNode *node,
    OCI_Date     *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_DATE, value);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_DATE) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    if (OcilibJsonNodeSetTemporal(node, JZNVAL_OCI_DATE, value->handle))
    {
        SET_SUCCESS();
    }

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetTimestamp
(
    OCI_JsonNode  *node,
    OCI_Timestamp *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_TIMESTAMP, value);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_TIMESTAMP) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    if (OcilibJsonNodeSetTemporal(node, JZNVAL_OCI_DATETIME, value->handle))
    {
        SET_SUCCESS();
    }

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetInterval
(
    OCI_JsonNode *node,
    OCI_Interval *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_INTERVAL, value);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(node->json->jdoc);
    if (node->type != OCI_JNT_INTERVAL) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    if (OcilibJsonNodeSetTemporal(node, JZNVAL_OCI_INTERVAL, value->handle))
    {
        SET_SUCCESS();
    }

    CLEANUP_AND_EXIT_FUNC();
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreate
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreate
(
    OCI_Json    *json,
    OCI_JsonType type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_JsonNode *node = NULL;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_NULL(json->jdoc);

    switch (type)
    {
        case OCI_JNT_NULL:
            node = OcilibJsonNodeCreateNull(json);
            break;
        case OCI_JNT_OBJECT:
            node = OcilibJsonNodeCreateObject(json);
            break;
        case OCI_JNT_ARRAY:
            node = OcilibJsonNodeCreateArray(json);
            break;
        case OCI_JNT_STRING:
            node = OcilibJsonNodeCreateString(json, OTEXT(""), 0);
            break;
        case OCI_JNT_NUMBER:
        {
            OCI_Number *num = OcilibNumberCreate(json->env);
            if (NULL != num)
            {
                node = OcilibJsonNodeCreateNumber(json, num);
                OcilibNumberFree(num);
            }
            break;
        }
        case OCI_JNT_BOOLEAN:
            node = OcilibJsonNodeCreateBoolean(json, FALSE);
            break;
        case OCI_JNT_BINARY:
            node = OcilibJsonNodeCreateBinary(json, NULL, 0);
            break;
        case OCI_JNT_DATE:
        {
            OCI_Date *date = OcilibDateCreate(json->env);
            if (NULL != date)
            {
                node = OcilibJsonNodeCreateDate(json, date);
                OcilibDateFree(date);
            }
            break;
        }
        case OCI_JNT_TIMESTAMP:
        {
            OCI_Timestamp *tsp = OcilibTimestampCreate(json->env, OCI_TIMESTAMP);
            if (NULL != tsp)
            {
                node = OcilibJsonNodeCreateTimestamp(json, tsp);
                OcilibTimestampFree(tsp);
            }
            break;
        }
        case OCI_JNT_INTERVAL:
        {
            OCI_Interval *itv = OcilibIntervalCreate(json->env, OCI_INTERVAL_YM);
            if (NULL != itv)
            {
                node = OcilibJsonNodeCreateInterval(json, itv);
                OcilibIntervalFree(itv);
            }
            break;
        }
        default:
            THROW_NO_ARGS(OcilibExceptionTypeNotCompatible);
            break;
    }

    CHECK_NULL(node);

    SET_RETVAL(node);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(type);
    CHECK(FALSE);
    EXIT_FUNC();
#endif
}
