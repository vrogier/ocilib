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

#define OCL_JSON_NODE_NUMBER_SETTER(otype)                  \
        ENTER_FUNC                                          \
        (                                                   \
    /* returns */ OCI_Boolean, FALSE,                       \
    /* context */ OCI_IPC_JSON_NODE, node                   \
        );                                                  \
                                                            \
        CHECK(OcilibJsonNodeSetValue(node, otype, &value)); \
        SET_SUCCESS();                                      \
        EXIT_FUNC();                            \


#define OCL_JSON_NODE_NUMBER_GETTER(otype)                 \
        ENTER_FUNC                                         \
        (                                                  \
    /* returns */ OCI_Boolean, FALSE,                      \
    /* context */ OCI_IPC_JSON_NODE, node                  \
        );                                                 \
                                                           \
        CHECK_PTR(OCI_IPC_VOID, value);                    \
        CHECK(OcilibJsonNodeGetValue(node, otype, value)); \
        SET_SUCCESS();                                     \
        EXIT_FUNC();

#if OCI_VERSION_COMPILE >= OCI_21_1

/* jzn calls whose NULL/absent result is legitimate (missing field, node without
   parent) still record an error in the DOM document. Left in place, that stale
   error makes the next mutating JZN call fail spuriously. */

static void JsonNodeDrainDomError
(
    JsonDomDoc *jdoc
)
{
    if (NULL != jdoc &&
        NULL != jdoc->mthds_jznDomDoc &&
        NULL != jdoc->mthds_jznDomDoc->jznDomGetError)
    {
        sword jerr = 0;

        JsonDomGetError(jdoc, &jerr, TRUE);
    }
}

#endif

static void JsonNodeCacheStr
(
    OCI_JsonNode  *node,
    const oratext *src,
    OCI_Size       src_len
)
{
    if (NULL != node->str)
    {
        OcilibMemoryFree(node->json->env, node->str);
    }
    node->str = NULL;

    node->str = (otext *) OcilibMemoryAlloc(node->json->env, OCI_IPC_VOID,
                                            src_len + 1, sizeof(otext), TRUE);
    if (NULL != node->str)
    {
        /* JZN oratext payloads are UTF-8 bytes whatever the environment
           charset — decode them properly on wide builds */

        OcilibStringUTF8ToNative((const char *) src, src_len, node->str, src_len + 1);
    }
}

static void JsonNodeDropCachedStr
(
    OCI_JsonNode *node
)
{
    if (NULL != node->str)
    {
        OcilibMemoryFree(node->json->env, node->str);
        node->str = NULL;
    }
}

static oratext * JsonNodeGetMbcsString
(
    OCI_Environment *env,
    const otext     *value,
    OCI_UInt32       length,
    OCI_Size        *ora_len
)
{
    OCI_Int32 dbsize = (length > 0) ? (OCI_Int32)(length * sizeof(otext)) : -1;
    char     *mbstr  = OcilibStringGetDBMbcsString(env, value, &dbsize);

    if (NULL != ora_len)
    {
        *ora_len = (OCI_Size) dbsize;
    }

    return (oratext *) mbstr;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSplice
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibJsonNodeSplice
(
    OCI_JsonNode *node,
    jznDomNode   *hnd
)
{

/* Repoint an attached node at a new DOM node: the parent (object field, array
   slot or document root) is updated so the document actually references the
   new content. Detached nodes have nothing to splice.*/

#if OCI_VERSION_COMPILE >= OCI_21_1

    jznDomDoc  *jdoc = node->json ? node->json->jdoc : NULL;
    OCI_Boolean ok   = TRUE;

    if (NULL != node->domparent && NULL != node->domkey)      /* object field */
    {
        ok = (0 == JsonDomPutFieldValue(jdoc, (jznDomObject *) node->domparent,
                                        node->domkey, node->domkeylen, hnd));
    }
    else if (NULL != node->domparent)                         /* array element */
    {
        /* jznDomPutItem inserts at pos — delete first for replace semantics */

        ok = (0 != JsonDomDeleteItem(jdoc, (jznDomArray *) node->domparent, node->domidx)) &&
             (0 != JsonDomPutItem(jdoc, (jznDomArray *) node->domparent, node->domidx, hnd));
    }
    else if (NULL != jdoc && NULL != node->handle &&
             node->handle == JsonDomGetRoot(jdoc))            /* document root */
    {
        ok = (0 == JsonDomSetRoot(jdoc, hnd));
    }

    if (!ok)
    {
        return FALSE;
    }

    JsonNodeDropCachedStr(node);

    return TRUE;

#else

    OCI_NOT_USED(node);
    OCI_NOT_USED(hnd);
    return FALSE;

#endif
}

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

    if (!OcilibJsonNodeSplice(node, (jznDomNode *) tmp))
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

    node->json      = json;
    node->handle    = handle;
    node->type      = type;
    node->str       = NULL;
    node->domparent = NULL;
    node->domkey    = NULL;
    node->domkeylen = 0;
    node->domidx    = 0;

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

    JsonNodeDropCachedStr(node);

    FREE(node->domkey);

    FREE(node);

    SET_SUCCESS();

    EXIT_FUNC();
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
    CHECK_JSON_DOC(node->json);

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

#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibJsonNodeGetString
(
    OCI_JsonNode *node,
    OCI_UInt32   *length
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_STRING) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    res = JsonDomGetScalarInfo(node->json->jdoc, (jznDomScalar*)node->handle, &sval);

    CHECK_JSON(node->json->jdoc, res == 0);
    CHECK(sval.type_jznScalarVal == JZNVAL_STRING);

    JsonNodeCacheStr(node, sval.val_jznScalarVal, (OCI_Size) sval.len_jznScalarVal);
    CHECK_NULL(node->str);

    if (NULL != length)
    {
        *length = (OCI_UInt32) sval.len_jznScalarVal;
    }

    SET_RETVAL((const otext *) node->str);
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        ;
    );
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

#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
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
    CHECK_JSON_DOC(node->json);

    count = JsonDomGetNumFields(node->json->jdoc, (jznDomObject*)node->handle);

    SET_RETVAL((OCI_UInt32) count);

#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetFieldName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibJsonNodeGetFieldName
(
    OCI_JsonNode *node,
    OCI_UInt32    index
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_OBJECT) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    if (index >= JsonDomGetNumFields(node->json->jdoc, (jznDomObject *)node->handle)) { THROW(OcilibExceptionOutOfBounds,
                                                                                              (OCI_Int32) index); }

    memset(&nvp, 0, sizeof(nvp));

    count = JsonDomGetFields(node->json->jdoc, (jznDomObject*)node->handle,
                             (ub4)index, 1, &nvp);

    CHECK_JSON(node->json->jdoc, count > 0);
    CHECK(nvp.name_jznDomNameValuePair.fnm_jznDomObjFieldName != NULL);

    JsonNodeCacheStr(node, nvp.name_jznDomNameValuePair.fnm_jznDomObjFieldName,
                     (OCI_Size) nvp.name_jznDomNameValuePair.len_jznDomObjFieldName);
    CHECK_NULL(node->str);

    SET_RETVAL((const otext *) node->str);

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(index);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        ;
    );
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
    oratext      *orakey  = NULL;
    OCI_Size      ora_len = 0;
    jznDomNode   *hnd     = NULL;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_STRING, name);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_OBJECT) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    orakey = JsonNodeGetMbcsString(node->json->env, name, 0, &ora_len);

    CHECK_NULL(orakey);

    hnd = JsonDomGetFieldByName(node->json->jdoc, (jznDomObject*)node->handle,
                                orakey, (ub2) ora_len);

    if (NULL == hnd)
    {
        JsonNodeDrainDomError(node->json->jdoc);
    }
    else
    {
        field = OcilibJsonNodeInitialize(node->json, hnd, OCI_JNT_NULL);

        CHECK_NULL(field);

        field->domparent = node->handle;

        ALLOC_BUFFER(OCI_IPC_VOID, field->domkey, ora_len + 1, 1);

        memcpy(field->domkey, orakey, ora_len);

        field->domkeylen = (ub2) ora_len;
    }

    SET_RETVAL(field);

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(name);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (

        OcilibStringReleaseDBMbcsString(node->json->env, (char *) orakey);

        if (FAILURE && NULL != field)
        {
            OcilibJsonNodeFree(field);
            field = NULL;
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetFieldByIndex
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeGetFieldByIndex
(
    OCI_JsonNode *node,
    OCI_UInt32    index
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_OBJECT) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    if (index >= JsonDomGetNumFields(node->json->jdoc, (jznDomObject *)node->handle)) { THROW(OcilibExceptionOutOfBounds,
                                                                                              (OCI_Int32) index); }

    memset(&nvp, 0, sizeof(nvp));

    count = JsonDomGetFields(node->json->jdoc, (jznDomObject*)node->handle,
                             (ub4)index, 1, &nvp);

    CHECK_JSON(node->json->jdoc, count > 0);
    CHECK(nvp.node_jznDomNameValuePair != NULL);

    field = OcilibJsonNodeInitialize(node->json, nvp.node_jznDomNameValuePair, OCI_JNT_NULL);

    CHECK_NULL(field);

    field->domparent = node->handle;

    ALLOC_BUFFER(OCI_IPC_VOID, field->domkey, nvp.name_jznDomNameValuePair.len_jznDomObjFieldName + 1, 1);

    memcpy(field->domkey, nvp.name_jznDomNameValuePair.fnm_jznDomObjFieldName,
           nvp.name_jznDomNameValuePair.len_jznDomObjFieldName);

    field->domkeylen = (ub2) nvp.name_jznDomNameValuePair.len_jznDomObjFieldName;

    SET_RETVAL(field);

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(index);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != field)
        {
            OcilibJsonNodeFree(field);
            field = NULL;
        }
    );
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
    CHECK_JSON_DOC(node->json);

    count = JsonDomGetArraySize(node->json->jdoc, (jznDomArray*)node->handle);

    SET_RETVAL((OCI_UInt32) count);

#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetItem
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeGetItem
(
    OCI_JsonNode *node,
    OCI_UInt32    index
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_ARRAY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    if (index >= JsonDomGetArraySize(node->json->jdoc, (jznDomArray *)node->handle)) { THROW(OcilibExceptionOutOfBounds, (OCI_Int32) index);}

    hnd = JsonDomGetArrayElement(node->json->jdoc,
                                 (jznDomArray*)node->handle,
                                 (ub4)index);

    CHECK_JSON(node->json->jdoc, NULL != hnd);

    item = OcilibJsonNodeInitialize(node->json, hnd, OCI_JNT_NULL);

    CHECK_NULL(item);

    item->domparent = node->handle;
    item->domidx    = index;

    SET_RETVAL(item);

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(index);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != item)
        {
            OcilibJsonNodeFree(item);
            item = NULL;
        }
    );
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_NUMBER) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    memset(&sval, 0, sizeof(sval));
    memset(&aux,  0, sizeof(aux));

    res = JsonDomGetScalarInfoOci(node->json->jdoc, (jznDomScalar*)node->handle,
                                  &sval, &aux);

    CHECK_JSON(node->json->jdoc, res == 0);
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
        OCI_Int32    dbsize  = -1;
        OCI_Int32    fmtsize = -1;

        CHECK_NULL(fmt);

        /* the DOM keeps the scalar text in oratext form; OCINumberFromText
           expects both the value and the format in the environment charset */

        otstr = (otext *) OcilibMemoryAlloc(node->json->env, OCI_IPC_VOID,
                                            (OCI_Size) sval.len_jznScalarVal + 1, sizeof(otext), FALSE);
        CHECK_NULL(otstr);
        OcilibStringMbcsToNative((const char *) sval.val_jznScalarVal, otstr,
                                 (OCI_Size) sval.len_jznScalarVal);
        otstr[sval.len_jznScalarVal] = 0;

        dbstr = (dbtext *) (otstr);
        dbsize = OcilibStringDBSize(otstr);
        dbfmt = (dbtext *) (fmt);
        fmtsize = OcilibStringDBSize(fmt);

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

#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        FREE(otstr);

        if (FAILURE && NULL != value)
        {
            OcilibNumberFree(value);
            value = NULL;
        }
    );
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_BOOLEAN) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    res = JsonDomGetScalarInfo(node->json->jdoc, (jznDomScalar*)node->handle, &sval);

    CHECK_JSON(node->json->jdoc, res == 0);
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

#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetBinary
 * --------------------------------------------------------------------------------------------- */

const void * OcilibJsonNodeGetBinary
(
    OCI_JsonNode *node,
    OCI_UInt32   *length
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_BINARY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    res = JsonDomGetScalarInfo(node->json->jdoc, (jznDomScalar*)node->handle, &sval);

    CHECK_JSON(node->json->jdoc, res == 0);
    CHECK(sval.type_jznScalarVal == JZNVAL_BINARY);

    value = (const void *) sval.binval_jznScalarVal;

    if (NULL != length)
    {
        *length = (OCI_UInt32) sval.binlen_jznScalarVal;
    }

    SET_RETVAL(value);

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(length);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_DATE) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    memset(&sval, 0, sizeof(sval));
    memset(&aux,  0, sizeof(aux));

    res = JsonDomGetScalarInfoOci(node->json->jdoc, (jznDomScalar*)node->handle,
                                  &sval, &aux);

    CHECK_JSON(node->json->jdoc, res == 0);
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

#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != value)
        {
            OcilibDateFree(value);
            value = NULL;
        }
    );
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_TIMESTAMP) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    memset(&sval, 0, sizeof(sval));
    memset(&aux,  0, sizeof(aux));

    res = JsonDomGetScalarInfoOci(node->json->jdoc, (jznDomScalar*)node->handle,
                                  &sval, &aux);

    CHECK_JSON(node->json->jdoc, res == 0);
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
        OCI_Size    tz_len    = 0;

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

#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != value)
        {
            OcilibTimestampFree(value);
            value = NULL;
        }
    );
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_INTERVAL) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    memset(&sval, 0, sizeof(sval));
    memset(&aux,  0, sizeof(aux));

    res = JsonDomGetScalarInfoOci(node->json->jdoc, (jznDomScalar*)node->handle,
                                  &sval, &aux);

    CHECK_JSON(node->json->jdoc, res == 0);
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

#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != value)
        {
            OcilibIntervalFree(value);
            value = NULL;
        }
    );
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
    CHECK_JSON_DOC(json);

    hnd = (jznDomNode *) JsonDomNewObject(json->jdoc, 0);

    CHECK_JSON(json->jdoc, NULL != hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_OBJECT);

    CHECK_NULL(node);

    SET_RETVAL(node);

#else
    OCI_NOT_USED(json);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
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
    CHECK_JSON_DOC(json);

    hnd = (jznDomNode *) JsonDomNewArray(json->jdoc, 0);

    CHECK_JSON(json->jdoc, NULL != hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_ARRAY);

    CHECK_NULL(node);

    SET_RETVAL(node);

#else
    OCI_NOT_USED(json);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
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
    CHECK_JSON_DOC(json);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = JZNVAL_NULL;

    hnd = (jznDomNode *) JsonDomNewScalar(json->jdoc, &sval);

    CHECK_JSON(json->jdoc, NULL != hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_NULL);

    CHECK_NULL(node);

    SET_RETVAL(node);

#else
    OCI_NOT_USED(json);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreateString
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreateString
(
    OCI_Json    *json,
    const otext *value,
    OCI_UInt32   length
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
    oratext      *orastr  = NULL;
    OCI_Size      ora_len = 0;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_STRING, value);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(json);

    orastr = JsonNodeGetMbcsString(json->env, value, length, &ora_len);

    CHECK_NULL(orastr);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = JZNVAL_STRING;
    sval.val_jznScalarVal  = orastr;
    sval.len_jznScalarVal  = (ub4) ora_len;

    hnd = (jznDomNode *) JsonDomNewScalar(json->jdoc, &sval);

    CHECK_JSON(json->jdoc, NULL != hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_STRING);

    CHECK_NULL(node);

    SET_RETVAL(node);

#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    OCI_NOT_USED(length);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (

        OcilibStringReleaseDBMbcsString(json->env, (char *) orastr);

        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
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

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_NUMBER, value);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(json);

    hnd = (jznDomNode *) JsonDomNewScalarVal(json->jdoc, JZNVAL_OCI_NUMBER,
                                             value->handle);

    CHECK_JSON(json->jdoc, NULL != hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_NUMBER);

    CHECK_NULL(node);

    SET_RETVAL(node);

#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
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
    CHECK_JSON_DOC(json);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = (0 != value) ? JZNVAL_TRUE : JZNVAL_FALSE;

    hnd = (jznDomNode *) JsonDomNewScalar(json->jdoc, &sval);

    CHECK_JSON(json->jdoc, NULL != hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_BOOLEAN);

    CHECK_NULL(node);

    SET_RETVAL(node);

#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeCreateBinary
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeCreateBinary
(
    OCI_Json   *json,
    const void *value,
    OCI_UInt32  length
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
    CHECK_JSON_DOC(json);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal   = JZNVAL_BINARY;
    sval.binval_jznScalarVal = (ub1 *) value;
    sval.binlen_jznScalarVal = (ub4) length;

    hnd = (jznDomNode *) JsonDomNewScalar(json->jdoc, &sval);

    CHECK_JSON(json->jdoc, NULL != hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_BINARY);

    CHECK_NULL(node);

    SET_RETVAL(node);

#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    OCI_NOT_USED(length);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetString
(
    OCI_JsonNode *node,
    const otext  *value,
    OCI_UInt32    length
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
    oratext     *orastr  = NULL;
    OCI_Size     ora_len = 0;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_STRING, value);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_STRING) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    orastr = JsonNodeGetMbcsString(node->json->env, value, length, &ora_len);

    CHECK_NULL(orastr);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = JZNVAL_STRING;
    sval.val_jznScalarVal  = orastr;
    sval.len_jznScalarVal  = (ub4) ora_len;

    hnd = (jznDomNode *) JsonDomNewScalar(node->json->jdoc, &sval);
    CHECK_JSON(node->json->jdoc, NULL != hnd);

    CHECK_JSON(node->json->jdoc, OcilibJsonNodeSplice(node, hnd));

    node->handle = hnd;

    SET_SUCCESS();

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    OCI_NOT_USED(length);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (

        OcilibStringReleaseDBMbcsString(node->json->env, (char *) orastr);
    );
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

    jznDomNode *hnd = NULL;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_NUMBER, value);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_NUMBER) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    hnd = (jznDomNode *) JsonDomNewScalarVal(node->json->jdoc, JZNVAL_OCI_NUMBER,
                                             value->handle);
    CHECK_JSON(node->json->jdoc, NULL != hnd);

    CHECK_JSON(node->json->jdoc, OcilibJsonNodeSplice(node, hnd));

    node->handle = hnd;

    SET_SUCCESS();

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_BOOLEAN) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = (0 != value) ? JZNVAL_TRUE : JZNVAL_FALSE;

    hnd = (jznDomNode *) JsonDomNewScalar(node->json->jdoc, &sval);
    CHECK_JSON(node->json->jdoc, NULL != hnd);

    CHECK_JSON(node->json->jdoc, OcilibJsonNodeSplice(node, hnd));

    node->handle = hnd;

    SET_SUCCESS();

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetBinary
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetBinary
(
    OCI_JsonNode *node,
    const void   *value,
    OCI_UInt32    length
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_BINARY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal   = JZNVAL_BINARY;
    sval.binval_jznScalarVal = (ub1 *) value;
    sval.binlen_jznScalarVal = (ub4) length;

    hnd = (jznDomNode *) JsonDomNewScalar(node->json->jdoc, &sval);
    CHECK_JSON(node->json->jdoc, NULL != hnd);

    CHECK_JSON(node->json->jdoc, OcilibJsonNodeSplice(node, hnd));

    node->handle = hnd;

    SET_SUCCESS();

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    OCI_NOT_USED(length);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
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
    CHECK_JSON_DOC(node->json);

    memset(&sval, 0, sizeof(sval));
    sval.type_jznScalarVal = JZNVAL_NULL;

    hnd = (jznDomNode *) JsonDomNewScalar(node->json->jdoc, &sval);
    CHECK_JSON(node->json->jdoc, NULL != hnd);

    CHECK_JSON(node->json->jdoc, OcilibJsonNodeSplice(node, hnd));

    node->handle = hnd;
    node->type   = OCI_JNT_NULL;

    SET_SUCCESS();

#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
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
    oratext *orakey  = NULL;
    OCI_Size ora_len = 0;

    CHECK_PTR(OCI_IPC_JSON_NODE, obj);
    CHECK_PTR(OCI_IPC_STRING, name);
    CHECK_PTR(OCI_IPC_JSON_NODE, value);
    CHECK_PTR(OCI_IPC_JSON, obj->json);
    CHECK_PTR(OCI_IPC_CONNECTION, obj->json->con);
    CHECK_FEATURE(obj->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(obj->json);
    if (obj->type != OCI_JNT_OBJECT) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }
    CHECK_COMPAT(value->json == obj->json);

    orakey = JsonNodeGetMbcsString(obj->json->env, name, 0, &ora_len);

    CHECK_NULL(orakey);

    CHECK_JSON(obj->json->jdoc,
               JsonDomPutFieldValue(obj->json->jdoc, (jznDomObject *)obj->handle,
                                    orakey, (ub2) ora_len, value->handle) == 0);

    SET_SUCCESS();

#else
    OCI_NOT_USED(obj);
    OCI_NOT_USED(name);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (

        OcilibStringReleaseDBMbcsString(obj->json->env, (char *) orakey);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetItem
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetItem
(
    OCI_JsonNode *arr,
    OCI_UInt32    index,
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
    CHECK_JSON_DOC(arr->json);
    if (arr->type != OCI_JNT_ARRAY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }
    CHECK_COMPAT(value->json == arr->json);
    if (index > JsonDomGetArraySize(arr->json->jdoc, (jznDomArray *)arr->handle)) { THROW(OcilibExceptionOutOfBounds, (OCI_Int32) index); }

    CHECK_JSON(arr->json->jdoc,
               JsonDomPutItem(arr->json->jdoc, (jznDomArray *)arr->handle,
                              (ub4)index, value->handle) != 0);

    SET_SUCCESS();

#else
    OCI_NOT_USED(arr);
    OCI_NOT_USED(index);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
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
    CHECK_JSON_DOC(arr->json);
    if (arr->type != OCI_JNT_ARRAY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }
    CHECK_COMPAT(value->json == arr->json);

    CHECK_JSON(arr->json->jdoc,
               JsonDomAppendItem(arr->json->jdoc, (jznDomArray *)arr->handle, value->handle) != 0);

    SET_SUCCESS();

#else
    OCI_NOT_USED(arr);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
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
    CHECK_JSON_DOC(json);

    hnd = (jznDomNode *) JsonDomNewScalarVal(json->jdoc, JZNVAL_OCI_DATE, value->handle);

    CHECK_JSON(json->jdoc, NULL != hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_DATE);

    CHECK_NULL(node);

    SET_RETVAL(node);

#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
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
    CHECK_JSON_DOC(json);

    hnd = (jznDomNode *) JsonDomNewScalarVal(json->jdoc, JZNVAL_OCI_DATETIME, value->handle);

    CHECK_JSON(json->jdoc, NULL != hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_TIMESTAMP);

    CHECK_NULL(node);

    SET_RETVAL(node);

#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
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
    CHECK_JSON_DOC(json);

    hnd = (jznDomNode *) JsonDomNewScalarVal(json->jdoc, JZNVAL_OCI_INTERVAL, value->handle);

    CHECK_JSON(json->jdoc, NULL != hnd);

    node = OcilibJsonNodeInitialize(json, hnd, OCI_JNT_INTERVAL);

    CHECK_NULL(node);

    SET_RETVAL(node);

#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetDate
 * --------------------------------------------------------------------------------------------- */
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_DATE) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    CHECK_JSON(node->json->jdoc,
               OcilibJsonNodeSetTemporal(node, JZNVAL_OCI_DATE, value->handle));

    SET_SUCCESS();

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_TIMESTAMP) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    CHECK_JSON(node->json->jdoc,
               OcilibJsonNodeSetTemporal(node, JZNVAL_OCI_DATETIME, value->handle));

    SET_SUCCESS();

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
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
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_INTERVAL) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    CHECK_JSON(node->json->jdoc,
               OcilibJsonNodeSetTemporal(node, JZNVAL_OCI_INTERVAL, value->handle));

    SET_SUCCESS();

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeInsertItem
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeInsertItem
(
    OCI_JsonNode *arr,
    OCI_UInt32    index,
    OCI_JsonNode *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, arr
    );

    jznDomNode *hnd = NULL;

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, arr);
    CHECK_PTR(OCI_IPC_JSON_NODE, value);
    CHECK_PTR(OCI_IPC_JSON, arr->json);
    CHECK_PTR(OCI_IPC_CONNECTION, arr->json->con);
    CHECK_FEATURE(arr->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(arr->json);
    if (arr->type != OCI_JNT_ARRAY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }
    CHECK_COMPAT(value->json == arr->json);
    if (index > JsonDomGetArraySize(arr->json->jdoc, (jznDomArray *)arr->handle) + 1) { THROW(OcilibExceptionOutOfBounds, (OCI_Int32) index); }

    hnd = value->handle;

    CHECK_JSON(arr->json->jdoc,
               JsonDomInsertElements(arr->json->jdoc, (jznDomArray *)arr->handle,
                                     (ub4) index, hnd) != 0);

    SET_SUCCESS();
#else
    OCI_NOT_USED(arr);
    OCI_NOT_USED(index);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeRemoveItem
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeRemoveItem
(
    OCI_JsonNode *arr,
    OCI_UInt32    index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, arr
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, arr);
    CHECK_PTR(OCI_IPC_JSON, arr->json);
    CHECK_PTR(OCI_IPC_CONNECTION, arr->json->con);
    CHECK_FEATURE(arr->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(arr->json);
    if (arr->type != OCI_JNT_ARRAY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }
    if (index > JsonDomGetArraySize(arr->json->jdoc, (jznDomArray *)arr->handle)) { THROW(OcilibExceptionOutOfBounds, (OCI_Int32) index); }

    CHECK_JSON(arr->json->jdoc,
               JsonDomDeleteItem(arr->json->jdoc, (jznDomArray *)arr->handle, (ub4) index) != 0);

    SET_SUCCESS();
#else
    OCI_NOT_USED(arr);
    OCI_NOT_USED(index);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeHasField
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeHasField
(
    OCI_JsonNode *node,
    const otext  *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );
    oratext *orakey  = NULL;
    OCI_Size ora_len = 0;

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_STRING, name);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_OBJECT) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    orakey = JsonNodeGetMbcsString(node->json->env, name, 0, &ora_len);

    CHECK_NULL(orakey);

    if (NULL != JsonDomGetFieldPos(node->json->jdoc, (jznDomObject *)node->handle,
                                   orakey, (ub2) ora_len))
    {
        SET_SUCCESS();
    }
    else
    {
        JsonNodeDrainDomError(node->json->jdoc);
    }
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(name);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (

        OcilibStringReleaseDBMbcsString(node->json->env, (char *) orakey);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeRemoveField
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeRemoveField
(
    OCI_JsonNode *node,
    const otext  *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );
    oratext *orakey  = NULL;
    OCI_Size ora_len = 0;

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_STRING, name);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_OBJECT) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    orakey = JsonNodeGetMbcsString(node->json->env, name, 0, &ora_len);

    CHECK_NULL(orakey);

    CHECK_JSON(node->json->jdoc,
               JsonDomDeleteFieldByName(node->json->jdoc, (jznDomObject *)node->handle,
                                        orakey, (ub2) ora_len) != 0);

    SET_SUCCESS();
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(name);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (

        OcilibStringReleaseDBMbcsString(node->json->env, (char *) orakey);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeClear
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeClear
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

    jznDomNameValuePair *nvps  = NULL;
    ub4                  count = 0;
    ub4                  i     = 0;

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(node->json);

    if (node->type == OCI_JNT_ARRAY)
    {
        count = JsonDomGetArraySize(node->json->jdoc, (jznDomArray *)node->handle);

        CHECK_JSON(node->json->jdoc,
                   JsonDomDeleteItems(node->json->jdoc, (jznDomArray *)node->handle,
                                      0, count) == count);
    }
    else if (node->type == OCI_JNT_OBJECT)
    {
        count = JsonDomGetNumFields(node->json->jdoc, (jznDomObject *)node->handle);

        if (count > 0)
        {
            nvps = OcilibMemoryAlloc(node->json->env, OCI_IPC_VOID, sizeof(*nvps), count, TRUE);

            CHECK_NULL(nvps);

            CHECK_JSON(node->json->jdoc,
                       count == JsonDomGetFields(node->json->jdoc, (jznDomObject *)node->handle,
                                                 0, count, nvps));

            for (i = 0; i < count; i++)
            {
                CHECK_JSON(node->json->jdoc,
                           JsonDomDeleteField(node->json->jdoc, (jznDomObject *)node->handle,
                                              &nvps[i].name_jznDomNameValuePair) != 0);
            }
        }
    }
    else
    {
        THROW_NO_ARGS(OcilibExceptionTypeNotCompatible);
    }

    SET_SUCCESS();
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != nvps)
        {
            OcilibMemoryFree(node->json->env, nvps);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetParent
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeGetParent
(
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode *, NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

    OCI_JsonNode *parent = NULL;

#if OCI_VERSION_COMPILE >= OCI_21_1

    jznDomNode *hnd = NULL;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(node->json);

    hnd = JsonDomGetParent(node->json->jdoc, (jznDomNode *)node->handle);

    if (NULL != hnd)
    {
        parent = OcilibJsonNodeInitialize(node->json, hnd, OCI_JNT_NULL);

        CHECK_NULL(parent);
    }
    else
    {
        JsonNodeDrainDomError(node->json->jdoc);
    }

    SET_RETVAL(parent);
#else
    OCI_NOT_USED(node);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != parent)
        {
            OcilibJsonNodeFree(parent);
            parent = NULL;
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetValue
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeGetValue
(
    OCI_JsonNode   *node,
    OCI_NumericType type,
    void           *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

    OCI_Number *num = NULL;

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_VOID, value);

    num = OcilibJsonNodeGetNumber(node);

    CHECK_NULL(num);

    CHECK(OcilibNumberGetValue(num, type, value));

    SET_SUCCESS();
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(type);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != num)
        {
            OcilibNumberFree(num);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetValue
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetValue
(
    OCI_JsonNode   *node,
    OCI_NumericType type,
    void           *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

    OCI_Number *num = NULL;

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_VOID, value);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(node->json);

    num = OcilibNumberCreate(node->json->env);

    CHECK_NULL(num);

    CHECK(OcilibNumberSetValue(num, type, value));

    CHECK(OcilibJsonNodeSetNumber(node, num));

    SET_SUCCESS();
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(type);
    OCI_NOT_USED(value);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != num)
        {
            OcilibNumberFree(num);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetByPath
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonNodeGetByPath
(
    OCI_JsonNode *node,
    const otext  *path
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode *, NULL,
        /* context */ OCI_IPC_JSON_NODE, node
    );

    OCI_JsonNode *cur   = NULL;
    OCI_JsonNode *next  = NULL;
    otext        *seg   = NULL;
    OCI_Boolean   found = FALSE;

#if OCI_VERSION_COMPILE >= OCI_21_1

    const otext *p = path;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_STRING, path);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(node->json);

    if (OTEXT('$') == *p)
    {
        p++;
    }

    cur = node;

    while (0 != *p)
    {
        OCI_Size seglen = 0;

        if (OTEXT('.') == *p)
        {
            p++;
        }

        if (OTEXT('[') == *p)
        {
            OCI_UInt32 idx = 0;

            p++;

            while (*p >= OTEXT('0') && *p <= OTEXT('9'))
            {
                idx = idx * 10 + (OCI_UInt32)(*p - OTEXT('0'));
                p++;
            }

            if (OTEXT(']') != *p) { THROW(OcilibExceptionParsingToken, *p); }
            p++;

            if (cur->type != OCI_JNT_ARRAY ||
                idx >= OcilibJsonNodeGetItemCount(cur))
            {
                goto not_found;
            }

            next = OcilibJsonNodeGetItem(cur, idx);

            CHECK_NULL(next);
        }
        else
        {
            const otext *start = p;

            while (0 != *p && OTEXT('.') != *p && OTEXT('[') != *p)
            {
                p++;
            }

            seglen = (OCI_Size)(p - start);

            if (0 == seglen) { THROW(OcilibExceptionParsingToken, *p); }

            seg = OcilibMemoryAlloc(node->json->env, OCI_IPC_VOID, sizeof(otext), seglen + 1, TRUE);

            CHECK_NULL(seg);

            memcpy(seg, start, sizeof(otext) * seglen);
            seg[seglen] = 0;

            if (cur->type != OCI_JNT_OBJECT ||
                FALSE == OcilibJsonNodeHasField(cur, seg))
            {
                goto not_found;
            }

            next = OcilibJsonNodeGetFieldByName(cur, seg);

            CHECK_NULL(next);

            FREE(seg);
        }

        if (cur != node)
        {
            OcilibJsonNodeFree(cur);
        }

        cur  = next;
        next = NULL;
    }

    if (cur != node)
    {
        found = TRUE;
        SET_RETVAL(cur);
        cur = NULL;
    }
    else if (0 == *p)
    {
        /* path resolved to the node itself ("$" or empty path) */
        cur = OcilibJsonNodeInitialize(node->json, (JsonDomNode *) node->handle, node->type);

        CHECK_NULL(cur);

        found = TRUE;
        SET_RETVAL(cur);
        cur = NULL;
    }

    (void)found;

not_found:

    /* call_retval stays at its ENTER_FUNC default (NULL); do NOT SET_RETVAL here —
       the success path above falls through to this label and must keep `cur`. */
    ;

#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(path);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != seg)
        {
            FREE(seg);
        }
        if (NULL != cur && cur != node)
        {
            OcilibJsonNodeFree(cur);
            cur = NULL;
        }
        if (NULL != next)
        {
            OcilibJsonNodeFree(next);
            next = NULL;
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetInt16
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeGetInt16
(
    OCI_JsonNode *node,
    OCI_Int16    *value
)
{
    OCL_JSON_NODE_NUMBER_GETTER(OCI_NUM_INT16);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetUInt16
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeGetUInt16
(
    OCI_JsonNode *node,
    OCI_UInt16   *value
)
{
    OCL_JSON_NODE_NUMBER_GETTER(OCI_NUM_UINT16);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetInt32
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeGetInt32
(
    OCI_JsonNode *node,
    OCI_Int32    *value
)
{
    OCL_JSON_NODE_NUMBER_GETTER(OCI_NUM_INT32);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetUInt32
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeGetUInt32
(
    OCI_JsonNode *node,
    OCI_UInt32   *value
)
{
    OCL_JSON_NODE_NUMBER_GETTER(OCI_NUM_UINT32);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetInt64
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeGetInt64
(
    OCI_JsonNode *node,
    OCI_Int64    *value
)
{
    OCL_JSON_NODE_NUMBER_GETTER(OCI_NUM_INT64);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetUInt64
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeGetUInt64
(
    OCI_JsonNode *node,
    OCI_UInt64   *value
)
{
    OCL_JSON_NODE_NUMBER_GETTER(OCI_NUM_UINT64);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetFloat
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeGetFloat
(
    OCI_JsonNode *node,
    OCI_Float    *value
)
{
    OCL_JSON_NODE_NUMBER_GETTER(OCI_NUM_FLOAT);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetDouble
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeGetDouble
(
    OCI_JsonNode *node,
    OCI_Double   *value
)
{
    OCL_JSON_NODE_NUMBER_GETTER(OCI_NUM_DOUBLE);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetInt16
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetInt16
(
    OCI_JsonNode *node,
    OCI_Int16     value
)
{
    OCL_JSON_NODE_NUMBER_SETTER(OCI_NUM_INT16);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetUInt16
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetUInt16
(
    OCI_JsonNode *node,
    OCI_UInt16    value
)
{
    OCL_JSON_NODE_NUMBER_SETTER(OCI_NUM_UINT16);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetInt32
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetInt32
(
    OCI_JsonNode *node,
    OCI_Int32     value
)
{
    OCL_JSON_NODE_NUMBER_SETTER(OCI_NUM_INT32);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetUInt32
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetUInt32
(
    OCI_JsonNode *node,
    OCI_UInt32    value
)
{
    OCL_JSON_NODE_NUMBER_SETTER(OCI_NUM_UINT32);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetInt64
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetInt64
(
    OCI_JsonNode *node,
    OCI_Int64     value
)
{
    OCL_JSON_NODE_NUMBER_SETTER(OCI_NUM_INT64);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetUInt64
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetUInt64
(
    OCI_JsonNode *node,
    OCI_UInt64    value
)
{
    OCL_JSON_NODE_NUMBER_SETTER(OCI_NUM_UINT64);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetFloat
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetFloat
(
    OCI_JsonNode *node,
    OCI_Float     value
)
{
    OCL_JSON_NODE_NUMBER_SETTER(OCI_NUM_FLOAT);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeSetDouble
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeSetDouble
(
    OCI_JsonNode *node,
    OCI_Double    value
)
{
    OCL_JSON_NODE_NUMBER_SETTER(OCI_NUM_DOUBLE);
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
    CHECK_JSON_DOC(json);

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

#else
    OCI_NOT_USED(json);
    OCI_NOT_USED(type);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != node)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeRemoveItems
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibJsonNodeRemoveItems
(
    OCI_JsonNode *arr,
    OCI_UInt32    start,
    OCI_UInt32    count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_JSON_NODE, arr
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    ub4 deleted = 0;

    CHECK_PTR(OCI_IPC_JSON_NODE, arr);
    CHECK_PTR(OCI_IPC_JSON, arr->json);
    CHECK_PTR(OCI_IPC_CONNECTION, arr->json->con);
    CHECK_FEATURE(arr->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(arr->json);
    if (arr->type != OCI_JNT_ARRAY) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    {
        const ub4 size = JsonDomGetArraySize(arr->json->jdoc, (jznDomArray *)arr->handle);

        CHECK_MIN(start, 1);
        CHECK_MIN(count, 1);

        if (start > size || start - 1 + count > size)
        {
            THROW(OcilibExceptionOutOfBounds, (OCI_Int32) start);
        }

        deleted = JsonDomDeleteItems(arr->json->jdoc, (jznDomArray *)arr->handle,
                                     (ub4) start-1, (ub4) count);

        CHECK_JSON(arr->json->jdoc, deleted == count);
    }

    SET_RETVAL((OCI_UInt32) deleted);
#else
    OCI_NOT_USED(arr);
    OCI_NOT_USED(start);
    OCI_NOT_USED(count);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeRenameField
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeRenameField
(
    OCI_JsonNode *node,
    const otext  *old_name,
    const otext  *new_name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    char     *dbold      = NULL;
    char     *dbnew      = NULL;
    OCI_Int32 dbold_size = -1;
    OCI_Int32 dbnew_size = -1;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_STRING, old_name);
    CHECK_PTR(OCI_IPC_STRING, new_name);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(node->json);
    if (node->type != OCI_JNT_OBJECT) { THROW_NO_ARGS(OcilibExceptionTypeNotCompatible); }

    dbold = OcilibStringGetDBMbcsString(node->json->env, old_name, &dbold_size);
    CHECK_NULL(dbold);

    dbnew = OcilibStringGetDBMbcsString(node->json->env, new_name, &dbnew_size);
    CHECK_NULL(dbnew);

    CHECK_JSON(node->json->jdoc,
               0 == JsonDomRenameField(node->json->jdoc, (jznDomObject *)node->handle,
                                       (oratext *) dbold, (ub2) dbold_size,
                                       (oratext *) dbnew, (ub2) dbnew_size));

    SET_SUCCESS();
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(old_name);
    OCI_NOT_USED(new_name);
    CHECK(FALSE);
#endif

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBMbcsString(node->json->env, dbold);
        OcilibStringReleaseDBMbcsString(node->json->env, dbnew);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeEquals
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeEquals
(
    OCI_JsonNode *node1,
    OCI_JsonNode *node2
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node1
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_JSON_NODE, node1);
    CHECK_PTR(OCI_IPC_JSON_NODE, node2);
    CHECK_PTR(OCI_IPC_JSON, node1->json);
    CHECK_PTR(OCI_IPC_JSON, node2->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node1->json->con);
    CHECK_FEATURE(node1->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(node1->json);
    CHECK_JSON_DOC(node2->json);

    if (node1 == node2)
    {
        SET_RETVAL(TRUE);
    }
    else
    {
        SET_RETVAL((OCI_Boolean) JsonDomEquals(node1->json->jdoc, (jznDomNode *)node1->handle,
                                               node2->json->jdoc, (jznDomNode *)node2->handle));
        JsonNodeDrainDomError(node1->json->jdoc);
    }
#else
    OCI_NOT_USED(node1);
    OCI_NOT_USED(node2);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeClone
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonNodeAssign
(
    OCI_JsonNode *node,
    OCI_JsonNode *src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON_NODE, node
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    jznDomNode *copied = NULL;

    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_JSON_NODE, src);
    CHECK_PTR(OCI_IPC_JSON, node->json);
    CHECK_PTR(OCI_IPC_JSON, src->json);
    CHECK_PTR(OCI_IPC_CONNECTION, node->json->con);
    CHECK_FEATURE(node->json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_JSON_DOC(node->json);
    CHECK_JSON_DOC(src->json);

    /* deep-copy src into node's document, then repoint the node at the copy
       — the previous DOM node is orphaned and reclaimed with the document */

    copied = JsonDomCopy(src->json->jdoc, (jznDomNode *)src->handle, node->json->jdoc);

    CHECK_JSON(node->json->jdoc, NULL != copied);

    CHECK_JSON(node->json->jdoc, OcilibJsonNodeSplice(node, (jznDomNode *) copied));

    node->handle = (void *) copied;
    node->type   = src->type;

    JsonNodeDropCachedStr(node);

    SET_SUCCESS();
#else
    OCI_NOT_USED(node);
    OCI_NOT_USED(src);
    CHECK(FALSE);
#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonNodeGetJson
 * --------------------------------------------------------------------------------------------- */

OCI_Json * OcilibJsonNodeGetJson
(
    OCI_JsonNode *node
)
{
    GET_PROP
    (
        /* result */ OCI_Json *, NULL,
        /* handle */ OCI_IPC_JSON_NODE, node,
        /* member */ json
    );
}
