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

#include "sodadocument.h"

#include "macros.h"
#include "memory.h"
#include "sodadatabase.h"
#include "stringutils.h"
#include "json.h"

/* --------------------------------------------------------------------------------------------- *
  * OcilibSodaDocumentGetStringAttribute
  * --------------------------------------------------------------------------------------------- */

static otext* OcilibSodaDocumentGetStringAttribute
(
    OCI_SodaDocument* doc,
    OCI_Int32         attr,
    otext          ** str
)
{
    ENTER_FUNC
    (
        /* returns */ otext*, NULL,
        /* context */ OCI_IPC_SODA_DOC, doc
    );

    dbtext* dbval  = NULL;
    ub4     dbsize = 0;
    otext * value  = NULL;

    CHECK_PTR(OCI_IPC_SODA_DOC, doc);

    if (NULL == *str)
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SODA_DOCUMENT,
            (ub4)attr,
            doc->handle,
            &dbval,
            &dbsize,
            doc->sdb->con->err
        );

        value = OcilibStringDuplicateFromDBString(doc->sdb->env, dbval, dbcharcount(dbsize));

        CHECK(NULL != value);

        *str = value;
    }

    SET_RETVAL(*str);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentRefreshContent
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibSodaDocumentRefreshContent
(
    OCI_SodaDocument* doc
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_DOC, doc
    );

    void      * data    = NULL;
    ub4         size    = 0;
    OCI_Boolean is_json = FALSE;

    CHECK_PTR(OCI_IPC_SODA_DOC, doc);

    if (doc->sdb->con->env->version_runtime >= OCI_23_4)
    {
        boolean flag      = 0;
        ub4     flag_size = sizeof(flag);
        sword   res       = doc->sdb->con->env->api->OCIAttrGet
                            (
                                (dvoid*)doc->handle,
                                (ub4)OCI_HTYPE_SODA_DOCUMENT,
                                (dvoid*)&flag,
                                (ub4*)&flag_size,
                                (ub4)OCI_ATTR_SODA_JSON_DESC,
                                (OCIError*)doc->sdb->con->err
                            );

        if (OCI_SUCCESS == res)
        {
            is_json = (flag != 0);
        }
    }

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_SODA_DOCUMENT,
        OCI_ATTR_SODA_CONTENT,
        doc->handle,
        &data,
        &size,
        doc->sdb->con->err
    );

    doc->content_is_json = is_json;
    doc->content         = data;
    doc->content_len     = is_json ? 0 : size;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDocument* OcilibSodaDocumentInitialize
(
    OCI_SodaDatabase * sdb,
    OCI_SodaDocument * doc,
    OCISodaDoc       * handle,
    const void       * content,
    OCI_UInt32         contentSize,
    const otext      * key,
    ub4                docFlags
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDocument*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCISodaDoc * newHandle      = handle;
    OCI_Json   * src            = NULL;
    OCI_Json   * dest           = NULL;
    const void * newContent     = content;
    OCI_UInt32   newContentSize = contentSize;
    dbtext     * dbkey          = NULL;
    OCI_Int32    dbkeylen       = -1;

    OCI_Boolean alloc = FALSE;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);

    alloc = (NULL == doc);

    ALLOC_HANDLE(OCI_IPC_SODA_DOC, doc);

    if (docFlags == OCI_SODA_JSON_DESC)
    {
        newContent     = NULL;
        newContentSize = 0;
    }

    if (NULL != newContent && 0 == newContentSize)
    {
        newContentSize = (OCI_UInt32) ostrlen((const otext *) newContent);
    }

    if (NULL == newHandle)
    {
        if (NULL != key)
        {
            dbkey = OcilibStringGetDBString(sdb->env, key, &dbkeylen);
        }

        /* an empty key must not be passed to OCISodaDocCreateWithKey() : */
        /* with a UTF-16 environment, it produces a document handle that  */
        /* crashes OCIHandleFree() after an insert-and-get (OCI bug)      */

        if (NULL != dbkey && dbkeylen > 0)
        {
            CHECK_OCI
            (
                sdb->con->err,
                OCISodaDocCreateWithKey,
                sdb->env->handle,
                newContent,
                (ub4)newContentSize,
                (const OraText*)dbkey,
                (ub4)dbkeylen,
                (ub4)docFlags,
                &newHandle,
                sdb->con->err,
                OCI_DEFAULT
            );
        }
        else
        {
            CHECK_OCI
            (
                sdb->con->err,
                OCISodaDocCreate,
                sdb->env->handle,
                newContent,
                (ub4)newContentSize,
                (ub4)docFlags,
                &newHandle,
                sdb->con->err,
                OCI_DEFAULT
            );
        }
    }

    doc->sdb    = sdb;
    doc->handle = newHandle;

    FREE(doc->key);
    FREE(doc->version);
    FREE(doc->created_on);
    FREE(doc->last_modified);
    FREE(doc->media_type);

    if (NULL != doc->content_text && doc->content_text != (otext*)doc->content)
    {
        FREE(doc->content_text);
    }

    if (NULL != doc->json)
    {
        OcilibJsonFree(doc->json);
        doc->json = NULL;
    }

    doc->content      = NULL;
    doc->content_len  = 0;
    doc->content_text = NULL;

    CHECK(OcilibSodaDocumentRefreshContent(doc));

    if (NULL == handle && OCI_SODA_JSON_DESC == docFlags && NULL != doc->content)
    {
        ALLOC_HANDLE(OCI_IPC_JSON, src);
        src = OcilibJsonInitialize(sdb->con, src, ((OCI_Json*)content)->handle);
        CHECK_NULL(src);

        ALLOC_HANDLE(OCI_IPC_JSON, dest);
        dest = OcilibJsonInitialize(sdb->con, dest, (OCIJson*)doc->content);
        CHECK_NULL(dest);

        CHECK(OcilibJsonClone(src, dest));
    }

    SET_RETVAL(doc);

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(sdb->env, dbkey);

        if (NULL != src)
        {
            OcilibJsonFree(src);
        }

        if (NULL != dest)
        {
            OcilibJsonFree(dest);
        }

        if (FAILURE && alloc && NULL != doc)
        {
            if (NULL != doc->sdb)
            {
                OcilibSodaDocumentFree(doc);
            }
            else
            {
                OcilibErrorResetSource(NULL, doc);
                FREE(doc);
            }
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentCreateFromText
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDocument* OcilibSodaDocumentCreateFromText
(
    OCI_SodaDatabase * sdb,
    const otext      * content,
    OCI_UInt32         contentSize,
    const otext      * key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDocument*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_SODA_ENABLED(sdb->con);

    SET_RETVAL(OcilibSodaDocumentInitialize(sdb, NULL, NULL, content, contentSize, key, OCI_DEFAULT));

    EXIT_FUNC();

}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentCreateFromJson
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDocument* OcilibSodaDocumentCreateFromJson
(
    OCI_SodaDatabase * sdb,
    OCI_Json         * json,
    const otext      * key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDocument*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_SODA_ENABLED(sdb->con);
    CHECK_FEATURE(sdb->con, OCI_FEATURE_JSON, OCI_23_4);

    SET_RETVAL(OcilibSodaDocumentInitialize(sdb, NULL, NULL, json, 0, key, OCI_SODA_JSON_DESC));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaDocumentFree
(
    OCI_SodaDocument* doc
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_DOC, doc
    );

    CHECK_PTR(OCI_IPC_SODA_DOC, doc);

    if (NULL != doc->json)
    {
        OcilibJsonFree(doc->json);
    }

    if (NULL != doc->handle)
    {
        OcilibMemoryFreeHandle(doc->sdb->env, (dvoid*)doc->handle, OCI_HTYPE_SODA_DOCUMENT);
    }

    OcilibErrorResetSource(NULL, doc);

    if (NULL != doc->content_text && doc->content_text != (otext*)doc->content)
    {
        FREE(doc->content_text);
    }

    doc->content = NULL;

    FREE(doc->key);
    FREE(doc->version);
    FREE(doc->created_on);
    FREE(doc->last_modified);
    FREE(doc->media_type);
    FREE(doc);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentGetKey
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaDocumentGetKey
(
    OCI_SodaDocument* doc
)
{
    return OcilibSodaDocumentGetStringAttribute(doc, OCI_ATTR_SODA_KEY, &doc->key);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentGetVersion
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaDocumentGetVersion
(
    OCI_SodaDocument* doc
)
{
    return OcilibSodaDocumentGetStringAttribute(doc, OCI_ATTR_SODA_VERSION, &doc->version);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentGetCreatedOn
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaDocumentGetCreatedOn
(
    OCI_SodaDocument* doc
)
{
    return OcilibSodaDocumentGetStringAttribute(doc, OCI_ATTR_SODA_CREATE_TIMESTAMP, &doc->created_on);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentGetLastModified
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaDocumentGetLastModified
(
    OCI_SodaDocument* doc
)
{
    return OcilibSodaDocumentGetStringAttribute(doc, OCI_ATTR_SODA_LASTMOD_TIMESTAMP, &doc->last_modified);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentGetMediaType
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaDocumentGetMediaType
(
    OCI_SodaDocument* doc
)
{
    return OcilibSodaDocumentGetStringAttribute(doc, OCI_ATTR_SODA_MEDIA_TYPE, &doc->media_type);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentRefreshMetadata
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaDocumentRefreshMetadata
(
    OCI_SodaDocument* doc,
    OCISodaDoc      * reshandle
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_DOC, doc
    );

    CHECK_PTR(OCI_IPC_SODA_DOC, doc);
    CHECK_PTR(OCI_IPC_SODA_DB, doc->sdb);

    if (NULL != reshandle)
    {
        OCISodaDoc* dochandle = doc->handle;

        doc->handle = reshandle;

        FREE(doc->key);
        FREE(doc->version);
        FREE(doc->created_on);
        FREE(doc->last_modified);
        FREE(doc->media_type);

        OcilibSodaDocumentGetKey(doc);
        OcilibSodaDocumentGetVersion(doc);
        OcilibSodaDocumentGetCreatedOn(doc);
        OcilibSodaDocumentGetLastModified(doc);
        OcilibSodaDocumentGetMediaType(doc);

        doc->handle = dochandle;

        OcilibMemoryFreeHandle(doc->sdb->env, (dvoid*)reshandle, OCI_HTYPE_SODA_DOCUMENT);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentGetText
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaDocumentGetText
(
    OCI_SodaDocument* doc
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_SODA_DOC, doc
    );

    OCI_Json* json = NULL;

    CHECK_PTR(OCI_IPC_SODA_DOC, doc);

    if (NULL == doc->content_text)
    {
        if (doc->content_is_json)
        {
            CHECK(NULL != doc->content);

            ALLOC_HANDLE(OCI_IPC_JSON, json);
            json = OcilibJsonInitialize(doc->sdb->con, json, (OCIJson*)doc->content);
            CHECK(NULL != json);

            doc->content_text_size = 0;

            CHECK(OcilibJsonToText(json, NULL, &doc->content_text_size, 0));

            ALLOC_DATA(OCI_IPC_STRING, doc->content_text, doc->content_text_size + 1);

            {
                OCI_UInt32 len = doc->content_text_size + 1;

                CHECK(OcilibJsonToText(json, doc->content_text, &len, 0));
            }

            OcilibJsonFree(json);
            json = NULL;
        }
        else
        {
#if defined(OCI_CHARSET_MBCS)

            doc->content_text_size = (ub4)dbcharcount(doc->content_len);

            ALLOC_DATA(OCI_IPC_STRING, doc->content_text, doc->content_text_size + 1);

            if (NULL != doc->content)
            {
                memcpy(doc->content_text, doc->content, (OCI_Size)doc->content_len);
            }

            doc->content_text[doc->content_text_size] = 0;

#else

            OCI_Int32 content_len = (OCI_Int32)dbcharcount(doc->content_len);

            if (NULL == doc->content || 0 == content_len)
            {
                doc->content_text = (otext*)OcilibMemoryAlloc(doc->sdb->env, OCI_IPC_STRING,
                                                              sizeof(otext), 1, TRUE);
                CHECK(NULL != doc->content_text);
                doc->content_text_size = 0;
            }
            else
            {
                doc->content_text = OcilibStringDuplicateFromDBString(doc->sdb->env,
                                                                      (const dbtext*)doc->content,
                                                                      content_len);
                CHECK(NULL != doc->content_text);

                doc->content_text_size = (ub4)content_len;
            }

#endif
        }
    }

    SET_RETVAL(doc->content_text);

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != json)
        {
            OcilibJsonFree(json);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentGetJson
 * --------------------------------------------------------------------------------------------- */

OCI_Json* OcilibSodaDocumentGetJson
(
    OCI_SodaDocument* doc
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Json*, NULL,
        /* context */ OCI_IPC_SODA_DOC, doc
    );

    CHECK_PTR(OCI_IPC_SODA_DOC, doc);
    CHECK_FEATURE(doc->sdb->con, OCI_FEATURE_JSON, OCI_23_4);

    if (!doc->content_is_json)
    {
        SET_RETVAL(NULL);
    }
    else
    {
        CHECK(NULL != doc->content);

        if (NULL == doc->json)
        {
            doc->json = OcilibJsonInitialize(doc->sdb->con, NULL, (OCIJson*)doc->content);
            CHECK(NULL != doc->json);
        }

        SET_RETVAL(doc->json);
    }

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentGetIsJson
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaDocumentGetIsJson
(
    OCI_SodaDocument *doc
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_DOC, doc
    );

    const otext *media_type = NULL;

    CHECK_PTR(OCI_IPC_SODA_DOC, doc);

    media_type = OcilibSodaDocumentGetMediaType(doc);

    /* documents with no explicit media type are JSON by default in SODA */

    if (NULL == media_type || 0 == ostrcmp(media_type, OTEXT("application/json")))
    {
        SET_SUCCESS();
    }

    CLEANUP_AND_EXIT_FUNC();
}
