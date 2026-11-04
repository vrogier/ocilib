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

#include "sodadoc.h"

#include "macros.h"
#include "memory.h"
#include "sodadb.h"
#include "stringutils.h"
#include "json.h"

/* --------------------------------------------------------------------------------------------- *
  * OcilibSodaDocGetStringAttribute
  * --------------------------------------------------------------------------------------------- */

static otext* OcilibSodaDocGetStringAttribute
(
    OCI_SodaDoc* doc,
    OCI_Int32    attr,
    otext     ** str
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
 * OcilibSodaDocRefreshContent
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibSodaDocRefreshContent
(
    OCI_SodaDoc* doc
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
 * OcilibSodaDocInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDoc* OcilibSodaDocInitialize
(
    OCI_SodaDb * sdb,
    OCI_SodaDoc* doc,
    OCISodaDoc * handle,
    const void * content,
    OCI_UInt32   contentSize,
    const otext* key,
    ub4          docFlags
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDoc*, NULL,
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

    CHECK(OcilibSodaDocRefreshContent(doc));

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
        OcilibStringReleaseDBString(NULL != sdb ? sdb->env : NULL, dbkey);

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
                OcilibSodaDocFree(doc);
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
 * OcilibSodaDocCreateFromText
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDoc* OcilibSodaDocCreateFromText
(
    OCI_SodaDb * sdb,
    const otext* content,
    OCI_UInt32   contentSize,
    const otext* key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDoc*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_SODA_ENABLED(sdb->con);

    SET_RETVAL(OcilibSodaDocInitialize(sdb, NULL, NULL, content, contentSize, key, OCI_DEFAULT));

    EXIT_FUNC();

}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocCreateFromJson
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDoc* OcilibSodaDocCreateFromJson
(
    OCI_SodaDb * sdb,
    OCI_Json   * json,
    const otext* key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDoc*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_PTR(OCI_IPC_JSON, json);
    CHECK_SODA_ENABLED(sdb->con);
    CHECK_FEATURE(sdb->con, OCI_FEATURE_JSON, OCI_23_4);

    SET_RETVAL(OcilibSodaDocInitialize(sdb, NULL, NULL, json, 0, key, OCI_SODA_JSON_DESC));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaDocFree
(
    OCI_SodaDoc* doc
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
 * OcilibSodaDocGetKey
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaDocGetKey
(
    OCI_SodaDoc* doc
)
{
    return OcilibSodaDocGetStringAttribute(doc, OCI_ATTR_SODA_KEY, &doc->key);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocGetVersion
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaDocGetVersion
(
    OCI_SodaDoc* doc
)
{
    return OcilibSodaDocGetStringAttribute(doc, OCI_ATTR_SODA_VERSION, &doc->version);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocGetCreatedOn
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaDocGetCreatedOn
(
    OCI_SodaDoc* doc
)
{
    return OcilibSodaDocGetStringAttribute(doc, OCI_ATTR_SODA_CREATE_TIMESTAMP, &doc->created_on);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocGetLastModified
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaDocGetLastModified
(
    OCI_SodaDoc* doc
)
{
    return OcilibSodaDocGetStringAttribute(doc, OCI_ATTR_SODA_LASTMOD_TIMESTAMP, &doc->last_modified);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocGetMediaType
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaDocGetMediaType
(
    OCI_SodaDoc* doc
)
{
    return OcilibSodaDocGetStringAttribute(doc, OCI_ATTR_SODA_MEDIA_TYPE, &doc->media_type);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocRefreshMetadata
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaDocRefreshMetadata
(
    OCI_SodaDoc* doc,
    OCISodaDoc * reshandle
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_DOC, doc
    );

    CHECK_PTR(OCI_IPC_SODA_DOC, doc);
    CHECK_PTR(OCI_IPC_SODA_DB, doc->sdb);

    if (NULL != reshandle && reshandle != doc->handle)
    {
        OCISodaDoc* dochandle = doc->handle;

        doc->handle = reshandle;

        FREE(doc->key);
        FREE(doc->version);
        FREE(doc->created_on);
        FREE(doc->last_modified);
        FREE(doc->media_type);

        OcilibSodaDocGetKey(doc);
        OcilibSodaDocGetVersion(doc);
        OcilibSodaDocGetCreatedOn(doc);
        OcilibSodaDocGetLastModified(doc);
        OcilibSodaDocGetMediaType(doc);

        doc->handle = dochandle;

        OcilibMemoryFreeHandle(doc->sdb->env, (dvoid*)reshandle, OCI_HTYPE_SODA_DOCUMENT);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocGetText
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaDocGetText
(
    OCI_SodaDoc* doc
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

            CHECK(OcilibJsonToText(json, NULL, &doc->content_text_size, 0, TRUE));

            ALLOC_DATA(OCI_IPC_STRING, doc->content_text, doc->content_text_size + 1);

            {
                OCI_UInt32 len = doc->content_text_size + 1;

                CHECK(OcilibJsonToText(json, doc->content_text, &len, 0, TRUE));
            }

            OcilibJsonFree(json);
            json = NULL;
        }
        else
        {
#if defined(OCI_CHARSET_ANSI)

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
 * OcilibSodaDocGetJson
 * --------------------------------------------------------------------------------------------- */

OCI_Json* OcilibSodaDocGetJson
(
    OCI_SodaDoc* doc
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
 * OcilibSodaDocGetIsJson
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaDocGetIsJson
(
    OCI_SodaDoc *doc
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_DOC, doc
    );

    const otext *media_type = NULL;

    CHECK_PTR(OCI_IPC_SODA_DOC, doc);

    media_type = OcilibSodaDocGetMediaType(doc);

    /* documents with no explicit media type are JSON by default in SODA */

    if (NULL == media_type || 0 == ostrcmp(media_type, OTEXT("application/json")))
    {
        SET_SUCCESS();
    }

    CLEANUP_AND_EXIT_FUNC();
}
