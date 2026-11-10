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

#include "json.h"
#include "jsonnode.h"

#include "array.h"
#include "macros.h"
#include "memory.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Json * OcilibJsonInitialize
(
    OCI_Connection *con,
    OCI_Json       *json,
    OCIJson        *handle
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Json*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_FEATURE(con, OCI_FEATURE_JSON, OCI_21_1);

    if (NULL == json)
    {
        ALLOC_HANDLE(OCI_IPC_JSON, json);
    }

    json->env    = con->env;
    json->con    = con;
    json->jdoc   = NULL;
    json->handle = handle;

    if (NULL == json->handle)
    {
        json->hstate = OCI_OBJECT_ALLOCATED;

        CHECK
        (
            OcilibMemoryAllocDescriptor
            (
                con->env,
                (dvoid  *) con->env->handle,
                (dvoid **) (void *) &json->handle,
                OCI_DTYPE_JSON
            )
        );
    }
    else if (OCI_OBJECT_ALLOCATED_ARRAY != json->hstate)
    {
        json->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

#else

    OCI_NOT_USED(con);
    OCI_NOT_USED(json);
    OCI_NOT_USED(handle);

    CHECK(FALSE);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibJsonFree(json);
            json = NULL;
        }

        SET_RETVAL(json);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Json * OcilibJsonCreate
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Json*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_FEATURE(con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    SET_RETVAL(OcilibJsonInitialize(con, NULL, NULL));

#else

    OCI_NOT_USED(con);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Json ** OcilibJsonArrayCreate
(
    OCI_Connection *con,
    OCI_UInt32      nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Json**, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_MIN(nbelem, 1);
    CHECK_FEATURE(con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    arr = OcilibArrayCreate
          (
              con->env, con, nbelem, OCI_CDT_JSON, 0,
              sizeof(OCIJson*), sizeof(OCI_Json),
              OCI_DTYPE_JSON, NULL
          );

    CHECK_NULL(arr);

    SET_RETVAL((OCI_Json**)arr->tab_obj);

#else

    OCI_NOT_USED(con);
    OCI_NOT_USED(nbelem);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != arr)
        {
            OcilibArrayDispose(arr);
            FREE(arr);
            arr = NULL;
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonFree
(
    OCI_Json *json
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON, json
    );

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    if (OCI_OBJECT_ALLOCATED == json->hstate && NULL != json->handle)
    {
        OcilibMemoryFreeDescriptor(json->con->env, (dvoid *) json->handle, OCI_DTYPE_JSON);
        json->handle = NULL;
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != json->hstate)
    {
        OcilibErrorResetSource(NULL, json);

        if (NULL != json->jdoc)
        {
            json->jdoc->mthds_jznDomDoc->jznDomDocFree(json->jdoc);
            json->jdoc = NULL;
        }

        FREE(json);
    }

#endif

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonArrayFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonArrayFree
(
    OCI_Json **jsons
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, jsons
    );

    CHECK_PTR(OCI_IPC_VOID, jsons);

    SET_RETVAL(OcilibArrayFreeFromHandles(jsons[0] ? jsons[0]->env : NULL, (void**)jsons));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibJsonGetConnection
(
    OCI_Json *json
)
{
    GET_PROP
    (
        /* result */ OCI_Connection*, NULL,
        /* handle */ OCI_IPC_JSON, json,
        /* member */ con
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonLoadText
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonLoadText
(
    OCI_Json          *json,
    const otext       *json_text,
    OCI_UInt32         length,
    OCI_JsonValidation validation,
    OCI_JsonEncoding   encoding
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON, json
    );

    dbtext   *dbstr  = NULL;
    OCI_Int32 dbsize = (length == 0 ? -1 : (OCI_Int32)(length * sizeof(otext)));

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_VOID, json_text);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    dbstr = OcilibStringGetDBString(json->con->env, json_text, &dbsize);

    CHECK_NULL(dbstr);

    CHECK_OCI
    (
        json->con->env->err,
        OCIJsonTextBufferParse,
        json->con->env->handle,
        json->handle,
        dbstr,
        (oraub8) dbsize,
        (ub4) validation,
        (ub2) encoding,
        json->con->env->err,
        OCI_DEFAULT
    );

    json->jdoc = NULL;

    SET_SUCCESS();

#else

    OCI_NOT_USED(json_text);
    OCI_NOT_USED(length);
    OCI_NOT_USED(validation);
    OCI_NOT_USED(encoding);
    OCI_NOT_USED(dbsize);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != dbstr)
        {
            OcilibStringReleaseDBString(json->con->env, dbstr);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonLoadBinary
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonLoadBinary
(
    OCI_Json   *json,
    const void *data,
    OCI_UInt32  length
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON, json
    );

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_VOID, data);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_OCI
    (
        json->con->env->err,
        OCIJsonBinaryBufferLoad,
        json->con->env->handle,
        json->handle,
        (ub1 *) data,
        (oraub8) length,
        json->con->env->err,
        OCI_DEFAULT
    );

    json->jdoc = NULL;

    SET_SUCCESS();

#else

    OCI_NOT_USED(data);
    OCI_NOT_USED(length);

#endif

    CLEANUP_AND_EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonToText
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonToText
(
    OCI_Json     *json,
    otext        *buffer,
    OCI_UInt32   *length,
    OCI_JsonPrint print
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON, json
    );

    dbtext         *dbstr        = NULL;
    OCI_Int32       dbsize       = 0;
    OCI_Int32       attempt      = 0;
    sword           res          = OCI_ERROR;
    oraub8          bytes        = 0;
    ub4             mode         = OCI_JSON_TEXT_ENV_NLS;
    const OCI_Int32 max_attempts = 16;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_VOID, length);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    do
    {
        if (NULL != dbstr)
        {
            FREE(dbstr);
        }

        dbsize = (0 == dbsize) ? 1024 : (dbsize * 2);
        attempt++;

        dbstr = (dbtext *) OcilibMemoryAlloc(json->con->env, OCI_IPC_STRING,
                                             sizeof(dbtext), (OCI_Size) dbsize + 1, FALSE);
        CHECK_NULL(dbstr);

        bytes = (oraub8) ((OCI_Size)(dbsize + 1) * sizeof(dbtext));

        res = json->con->env->api->OCIJsonToTextBuffer
              (
                  json->con->handle,
                  json->handle,
                  (OraText *) dbstr,
                  &bytes,
                  (ub4) print,
                  json->con->env->err,
                  mode
              );

        if (res < 0)
        {
            sb4 err_code = 0;

            OCI_ERROR_GET(json->con->env->err, &err_code, NULL, 0);

            if ((40781 == err_code) && (attempt < max_attempts))
            {
                continue;
            }

            OcilibExceptionOCI(&ocilib_call_context, json->con->env->err, res);
            CHECK(res >= 0);
        }
    }
    while (res < 0);

    dbsize = (OCI_Int32) (bytes / sizeof(dbtext));

    if (NULL == buffer)
    {
        *length = (OCI_UInt32) dbsize;
        SET_SUCCESS();
    }
    else
    {
        OCI_UInt32 outlen = (OCI_UInt32) dbsize;

        if (outlen > *length)
        {
            outlen = *length;
        }

        OcilibStringOracleToNative(dbstr, buffer, (OCI_Int32) outlen);
        buffer[outlen] = 0;
        *length        = outlen;

        SET_SUCCESS();
    }

#else

    OCI_NOT_USED(buffer);
    OCI_NOT_USED(print);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != dbstr)
        {
            FREE(dbstr);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonToBinary
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonToBinary
(
    OCI_Json   *json,
    void       *buffer,
    OCI_UInt32 *length
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON, json
    );

    oraub8 bytes = 0;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_VOID, length);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    if (NULL == buffer)
    {
        CHECK_OCI
        (
            json->con->env->err,
            OCIJsonBinaryLengthGet,
            json->con->handle,
            json->handle,
            &bytes,
            json->con->env->err,
            OCI_DEFAULT
        );

        *length = (OCI_UInt32) bytes;

        SET_SUCCESS();
    }
    else
    {
        bytes = (oraub8) *length;

        CHECK_OCI
        (
            json->con->env->err,
            OCIJsonToBinaryBuffer,
            json->con->handle,
            json->handle,
            (ub1 *) buffer,
            &bytes,
            json->con->env->err,
            OCI_DEFAULT
        );

        *length = (OCI_UInt32) bytes;

        SET_SUCCESS();
    }

#else

    OCI_NOT_USED(buffer);

#endif

    CLEANUP_AND_EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonClone
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonClone
(
    OCI_Json *src,
    OCI_Json *dest
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON, src
    );

    ub1 *bin = NULL;

    CHECK_PTR(OCI_IPC_JSON, src);
    CHECK_PTR(OCI_IPC_JSON, dest);
    CHECK_PTR(OCI_IPC_CONNECTION, src->con);
    CHECK_FEATURE(src->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    OCI_UInt32 bin_len = 0;

    CHECK(OcilibJsonToBinary(src, NULL, &bin_len));
    CHECK(bin_len > 0);

    bin = (ub1 *) OcilibMemoryAlloc(src->con->env, OCI_IPC_VOID, (OCI_Size) bin_len, 1, FALSE);
    CHECK_PTR(OCI_IPC_VOID, bin);

    {
        OCI_UInt32 bin_len2 = bin_len;
        CHECK(OcilibJsonToBinary(src, bin, &bin_len2));
    }

    CHECK(OcilibJsonLoadBinary(dest, bin, bin_len));

    OcilibMemoryFree(src->con->env, bin);
    bin = NULL;

    SET_SUCCESS();

#else

    OCI_NOT_USED(dest);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != bin)
        {
            OcilibMemoryFree(src->con->env, bin);
            bin = NULL;
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonAssign
(
    OCI_Json *json,
    OCI_Json *json_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON, json
    );

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_JSON, json_src);

    SET_RETVAL(OcilibJsonClone(json_src, json));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonSetMutable
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonSetMutable
(
    OCI_Json   *json,
    OCI_Boolean value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON, json
    );

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    {
        ub1 flag = (ub1) value;

        CHECK_ATTRIB_SET
        (
            OCI_DTYPE_JSON, OCI_ATTR_JSON_DOM_MUTABLE,
            json->handle, &flag, sizeof(flag),
            json->con->env->err
        );
    }

    SET_SUCCESS();

#else

    OCI_NOT_USED(value);

#endif

    CLEANUP_AND_EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonIsMutable
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonIsMutable
(
    OCI_Json *json
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON, json
    );

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    {
        boolean flag = 0;
        ub4     size = sizeof(flag);

        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_JSON, OCI_ATTR_JSON_DOM_MUTABLE,
            json->handle, &flag, &size,
            json->con->env->err
        );

        SET_RETVAL((OCI_Boolean) flag);
    }

#else

    OCI_NOT_USED(json);

#endif

    CLEANUP_AND_EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonEnsureDoc
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonEnsureDoc
(
    OCI_Json *json
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON, json
    );

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    if (NULL == json->jdoc)
    {
        CHECK_OCI
        (
            json->con->env->err,
            OCIJsonDomDocGet,
            json->con->handle,
            json->handle,
            (JsonDomDoc**) &json->jdoc,
            json->con->env->err,
            OCI_DEFAULT
        );
    }

    CHECK_NULL(json->jdoc);

    SET_SUCCESS();

#else

    OCI_NOT_USED(json);

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonGetRoot
 * --------------------------------------------------------------------------------------------- */

OCI_JsonNode * OcilibJsonGetRoot
(
    OCI_Json *json
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_JsonNode*, NULL,
        /* context */ OCI_IPC_JSON, json
    );

    OCI_JsonNode *node = NULL;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_JSON_DOC(json);

    {
        jznDomNode *root = JsonDomGetRoot(json->jdoc);
        CHECK_JSON(json->jdoc, NULL != root);

        node = OcilibJsonNodeInitialize(json, root, OCI_JNT_NULL);
        CHECK_NULL(node);
    }

    SET_RETVAL(node);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibJsonNodeFree(node);
            node = NULL;
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonSetRoot
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonSetRoot
(
    OCI_Json     *json,
    OCI_JsonNode *node
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON, json
    );

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_JSON_NODE, node);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_21_1);
    CHECK_COMPAT(node->json == json);

#if OCI_VERSION_COMPILE >= OCI_21_1

    CHECK_JSON_DOC(json);

    {
        sword res = 0;

        res = JsonDomSetRoot(json->jdoc, node->handle);

        CHECK_JSON(json->jdoc, res == 0);
    }

    SET_SUCCESS();

#endif

    CLEANUP_AND_EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibJsonValidate
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibJsonValidate
(
    OCI_Json  * json,
    OCI_Json  * schema,
    OCI_Json ** errors
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_JSON, json
    );

    OCI_Json *errdoc = NULL;

    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_PTR(OCI_IPC_JSON, schema);
    CHECK_PTR(OCI_IPC_CONNECTION, json->con);
    CHECK_FEATURE(json->con, OCI_FEATURE_JSON, OCI_23_4);

#if OCI_VERSION_COMPILE >= OCI_21_1

    errdoc = OcilibJsonInitialize(json->con, NULL, NULL);
    CHECK_NULL(errdoc);

    CHECK_OCI
    (
        json->con->env->err,
        OCIJsonSchemaValidate,
        json->con->handle,
        json->handle,
        schema->handle,
        0,
        errdoc->handle,
        OCI_JSON_SCHEMA_ERROR_BASIC,
        json->con->err,
        OCI_DEFAULT
    );

    OCI_JsonNode *root  = OcilibJsonGetRoot(errdoc);
    OCI_JsonNode *vflag = (NULL == root)  ? NULL : OcilibJsonNodeGetFieldByName(root, OTEXT("valid"));

    if (NULL != vflag)
    {
        SET_RETVAL(OcilibJsonNodeGetBoolean(vflag));
        OcilibJsonNodeFree(vflag);
    }

    if (NULL != root)
    {
        OcilibJsonNodeFree(root);
    }

    if (NULL != errors)
    {
        *errors = errdoc;
        errdoc  = NULL;
    }

#else

    OCI_NOT_USED(errors);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != errdoc)
        {
            OcilibJsonFree(errdoc);
        }
    );
}
