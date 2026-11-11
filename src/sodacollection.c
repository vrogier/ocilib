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

#include "sodacollection.h"

#include "macros.h"
#include "memory.h"
#include "sodadatabase.h"
#include "sodadocument.h"
#include "sodadocumentcursor.h"
#include "sodaoptions.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
  * OcilibSodaCollectionGetStringAttribute
  * --------------------------------------------------------------------------------------------- */

static otext* OcilibSodaCollectionGetStringAttribute
(
    OCI_SodaCollection* coll,
    OCI_Int32           attr,
    otext            ** str
)
{
    ENTER_FUNC
    (
        /* returns */ otext*, NULL,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    dbtext* dbval  = NULL;
    ub4     dbsize = 0;
    otext * value  = NULL;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);

    if (NULL == *str)
    {
        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_SODA_COLLECTION,
            (ub4)attr,
            coll->handle,
            &dbval,
            &dbsize,
            coll->sdb->con->err
        );

        value = OcilibStringDuplicateFromDBString(coll->sdb->env, dbval, dbcharcount(dbsize));

        CHECK(NULL != value);

        *str = value;
    }

    SET_RETVAL(*str);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionRefreshContentInfo
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibSodaCollectionRefreshContentInfo
(
    OCI_SodaCollection* coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    ub1 sql_type = 0;
    ub1 fmt      = 0;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_SODA_COLLECTION,
        OCI_ATTR_SODA_CTNT_SQL_TYPE,
        coll->handle,
        &sql_type,
        NULL,
        coll->sdb->con->err
    );

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_SODA_COLLECTION,
        OCI_ATTR_SODA_CTNT_FORMAT,
        coll->handle,
        &fmt,
        NULL,
        coll->sdb->con->err
    );

    coll->content_sql_type = sql_type;
    coll->content_format   = fmt;
    coll->binary_content   = (sql_type == SQLT_BLOB ||
        sql_type == SQLT_BIN ||
        (sql_type == SQLT_JSON && fmt == OCI_JSON_FORMAT_OSON));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_SodaCollection* OcilibSodaCollectionInitialize
(
    OCI_SodaCollection* coll,
    OCI_SodaDatabase  * sdb,
    OCISodaColl       * handle
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaCollection*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCI_Boolean alloc = FALSE;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_PTR(OCI_IPC_VOID, handle);

    if (NULL == coll)
    {
        ALLOC_HANDLE(OCI_IPC_SODA_COLL, coll);
        alloc = TRUE;
    }

    coll->sdb    = sdb;
    coll->handle = handle;

    CHECK(OcilibSodaCollectionRefreshContentInfo(coll));

    SET_RETVAL(coll);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && alloc && NULL != coll)
        {
            OcilibMemoryFreeHandle(sdb->env, (dvoid*)coll->handle, OCI_HTYPE_SODA_COLLECTION);
            OcilibErrorResetSource(NULL, coll);
            FREE(coll);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionGetName
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaCollectionGetName
(
    OCI_SodaCollection* coll
)
{
    return OcilibSodaCollectionGetStringAttribute(coll, OCI_ATTR_SODA_COLL_NAME, &coll->name);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionGetMetadata
 * --------------------------------------------------------------------------------------------- */

const otext* OcilibSodaCollectionGetMetadata
(
    OCI_SodaCollection* coll
)
{
    return OcilibSodaCollectionGetStringAttribute(coll, OCI_ATTR_SODA_DESCRIPTOR, &coll->descriptor);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection* OcilibSodaCollectionGetConnection
(
    OCI_SodaCollection* coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Connection*, NULL,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_PTR(OCI_IPC_CONNECTION, coll->sdb->con);

    SET_RETVAL(coll->sdb->con);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionIsMapped
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionIsMapped
(
    OCI_SodaCollection* coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    OCI_Boolean mapped = FALSE;
    otext     * view   = NULL;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);

    if (coll->sdb->env->version_runtime >= OCI_23_4)
    {
        (void)OcilibSodaCollectionGetStringAttribute(coll, OCI_ATTR_SODA_VIEW_NAME, &view);

        mapped = IS_STRING_VALID(view);
    }

    SET_RETVAL(mapped);

    CLEANUP_AND_EXIT_FUNC
    (
        FREE(view);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionDrop
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionDrop
(
    OCI_SodaCollection* coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    boolean dropped = FALSE;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    CHECK_OCI
    (
        coll->sdb->con->err,
        OCISodaCollDrop,
        coll->sdb->con->handle,
        coll->handle,
        &dropped,
        coll->sdb->con->err,
        (ub4)OCI_DEFAULT
    );

    if (dropped)
    {
        OcilibMemoryFreeHandle(coll->sdb->env, (dvoid*)coll->handle, OCI_HTYPE_SODA_COLLECTION);
        coll->handle = NULL;
    }

    SET_RETVAL((OCI_Boolean)dropped);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionTruncate
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionTruncate
(
    OCI_SodaCollection* coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);
    CHECK_FEATURE(coll->sdb->con, OCI_FEATURE_SODA, OCI_20_1);

    CHECK_OCI
    (
        coll->sdb->con->err,
        OCISodaCollTruncate,
        coll->sdb->con->handle,
        coll->handle,
        coll->sdb->con->err,
        (ub4)OCI_DEFAULT
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionFree
(
    OCI_SodaCollection* coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);

    if (NULL != coll->handle)
    {
        OcilibMemoryFreeHandle(coll->sdb->env, (dvoid*)coll->handle, OCI_HTYPE_SODA_COLLECTION);
    }

    OcilibErrorResetSource(NULL, coll);

    FREE(coll->name);
    FREE(coll->descriptor);
    FREE(coll);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionInsertDocument
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionInsertDocument
(
    OCI_SodaCollection* coll,
    OCI_SodaDocument  * doc
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    OCISodaDoc* reshandle = NULL;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_SODA_DOC, doc);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    reshandle = doc->handle;

    CHECK_OCI
    (
        coll->sdb->con->err,
        OCISodaInsertAndGet,
        coll->sdb->con->handle,
        coll->handle,
        &reshandle,
        coll->sdb->con->err,
        (ub4)OCI_DEFAULT
    );

    CHECK(OcilibSodaDocumentRefreshMetadata(doc, reshandle));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionFind
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDocumentCursor* OcilibSodaCollectionFind
(
    OCI_SodaCollection * coll,
    OCI_SodaOptions    * options
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDocumentCursor*, NULL,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    OCI_SodaDocumentCursor * cursor    = NULL;
    OCI_SodaOptions        * tmpopts   = NULL;
    OCISodaOperationOptions* opthandle = NULL;
    OCISodaDoc             **mat_docs  = NULL;
    OCI_UInt32               mat_count = 0;

    OCI_UInt32 mat_cap   = 0;
    OCI_UInt32 key_count = 0;
    OCI_UInt32 k         = 0;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    ALLOC_HANDLE(OCI_IPC_SODA_DOC_CURSOR, cursor);
    cursor->coll = coll;

    if (NULL != options)
    {
        opthandle = options->handle;

        /* multi-key: OCISodaOperKeysSet (ANSI only — OraText** is unusable
           in UTF-16) pushes the key array onto the options handle for a
           single server-side op; wide builds keep the client-side per-key
           emulation below */

        key_count = options->key_count;

    }
    else
    {
        tmpopts = OcilibSodaOptionsCreate(coll->sdb);
        CHECK_NULL(tmpopts);

        opthandle = tmpopts->handle;
    }

    if (key_count > 0)
    {
        for (k = 0; k < key_count; k++)
        {
            OCISodaDocCursor *oci_cur = NULL;

            CHECK(OcilibSodaOptionsSetKeyAttr(options, options->keys[k]));

            CHECK_OCI
            (
                coll->sdb->con->err,
                OCISodaFind,
                coll->sdb->con->handle,
                coll->handle,
                options->handle,
                (ub4)OCILIB_SODA_DOC_FLAGS(coll->sdb),
                &oci_cur,
                coll->sdb->con->err,
                OCI_DEFAULT
            );

            for (;;)
            {
                OCISodaDoc *dh = NULL;
                sword       oci_ret;

                OCI_CALL(oci_ret, OCISodaDocGetNext,
                         coll->sdb->con->handle,
                         oci_cur,
                         &dh,
                         coll->sdb->con->err,
                         OCI_DEFAULT
                         );

                if (OCI_NO_DATA == oci_ret || NULL == dh)
                {
                    break;
                }

                if (OCI_SUCCESS != oci_ret && OCI_SUCCESS_WITH_INFO != oci_ret)
                {
                    OcilibMemoryFreeHandle(coll->sdb->env, (dvoid*)oci_cur, OCI_HTYPE_SODA_DOC_CURSOR);
                    THROW(OcilibExceptionOCI, coll->sdb->con->err, oci_ret);
                }

                if (mat_count >= mat_cap)
                {
                    OCISodaDoc **newdocs = OcilibMemoryAlloc
                                           (
                                               coll->sdb->env, OCI_IPC_VOID,
                                               sizeof(OCISodaDoc*),
                                               mat_cap + 8, TRUE
                                           );

                    if (NULL == newdocs)
                    {
                        OcilibMemoryFreeHandle(coll->sdb->env, (dvoid*)oci_cur, OCI_HTYPE_SODA_DOC_CURSOR);
                        OcilibMemoryFreeHandle(coll->sdb->env, (dvoid*)dh, OCI_HTYPE_SODA_DOCUMENT);
                        CHECK(FALSE);
                    }

                    if (NULL != mat_docs)
                    {
                        memcpy(newdocs, mat_docs, mat_count * sizeof(OCISodaDoc*));
                        OcilibMemoryFree(coll->sdb->env, mat_docs);
                    }

                    mat_docs = newdocs;
                    mat_cap += 8;
                }

                mat_docs[mat_count++] = dh;
            }

            OcilibMemoryFreeHandle(coll->sdb->env, (dvoid*)oci_cur, OCI_HTYPE_SODA_DOC_CURSOR);
        }

        cursor->docs      = mat_docs;
        cursor->doc_count = mat_count;
        cursor->doc_pos   = 0;
        mat_docs          = NULL;

        SET_RETVAL(cursor);
    }
    else

    {
        CHECK_OCI
        (
            coll->sdb->con->err,
            OCISodaFind,
            coll->sdb->con->handle,
            coll->handle,
            opthandle,
            (ub4)OCILIB_SODA_DOC_FLAGS(coll->sdb),
            &cursor->handle,
            coll->sdb->con->err,
            OCI_DEFAULT
        );

        CHECK(NULL != OcilibSodaDocumentCursorInitialize(cursor, coll, cursor->handle));

        SET_RETVAL(cursor);
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != tmpopts)
        {
            OcilibSodaOptionsFree(tmpopts);
        }

        if (FAILURE)
        {
            OCI_Environment *env = coll->sdb->env;
            OCI_UInt32 d;

            for (d = 0; d < mat_count; d++)
            {
                if (NULL != mat_docs && NULL != mat_docs[d])
                {
                    OcilibMemoryFreeHandle(env, (dvoid*)mat_docs[d], OCI_HTYPE_SODA_DOCUMENT);
                }
            }

            if (NULL != mat_docs)
            {
                OcilibMemoryFree(env, mat_docs);
            }

            if (NULL != cursor)
            {
                if (NULL != cursor->handle)
                {
                    OcilibMemoryFreeHandle(env, (dvoid*)cursor->handle, OCI_HTYPE_SODA_DOC_CURSOR);
                }

                OcilibErrorResetSource(NULL, cursor);
                FREE(cursor);
            }
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionFindOne
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDocument* OcilibSodaCollectionFindOne
(
    OCI_SodaCollection * coll,
    OCI_SodaOptions    * options
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDocument*, NULL,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    OCI_SodaDocument       * doc       = NULL;
    OCI_SodaOptions        * tmpopts   = NULL;
    OCISodaOperationOptions* opthandle = NULL;
    sword                    oci_ret   = 0;
    OCI_UInt32               loops     = 1;
    OCI_UInt32               i         = 0;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    if (NULL != options)
    {
        opthandle = options->handle;

        /* multi-key emulation: try each cached key until one matches */

        if (options->key_count > 0)
        {
            loops = options->key_count;
        }

    }
    else
    {
        tmpopts = OcilibSodaOptionsCreate(coll->sdb);
        CHECK_NULL(tmpopts);

        opthandle = tmpopts->handle;
    }

    ALLOC_HANDLE(OCI_IPC_SODA_DOC, doc);
    doc->sdb = coll->sdb;

    for (i = 0; i < loops; i++)
    {
        doc->handle = NULL;

        if (NULL != options && options->key_count > 0)
        {
            CHECK(OcilibSodaOptionsSetKeyAttr(options, options->keys[i]));
        }

        OCI_CALL(oci_ret, OCISodaFindOne, coll->sdb->con->handle,
                 coll->handle,
                 opthandle,
                 (ub4)OCILIB_SODA_DOC_FLAGS(coll->sdb),
                 &doc->handle,
                 coll->sdb->con->err,
                 OCI_DEFAULT
                 );

        if (OCI_NO_DATA == oci_ret || NULL == doc->handle)
        {
            continue;
        }

        CHECK(OCI_SUCCESS == oci_ret || OCI_SUCCESS_WITH_INFO == oci_ret);
        break;
    }

    if (NULL == doc->handle)
    {
        OcilibErrorResetSource(NULL, doc);
        FREE(doc);
        doc = NULL;
        JUMP_CLEANUP();
    }

    CHECK(NULL != OcilibSodaDocumentInitialize(doc->sdb, doc, doc->handle, NULL, 0, NULL, 0));

    SET_RETVAL(doc);

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != tmpopts)
        {
            OcilibSodaOptionsFree(tmpopts);
        }

        if (FAILURE && NULL != doc)
        {
            OcilibSodaDocumentFree(doc);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionFindOneByKey
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDocument* OcilibSodaCollectionFindOneByKey
(
    OCI_SodaCollection* coll,
    const otext       * key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDocument*, NULL,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    OCI_SodaDocument* doc      = NULL;
    dbtext          * dbkey    = NULL;
    OCI_Int32         dbkeylen = -1;
    sword             oci_ret  = 0;
    OCISodaDoc      * handle   = NULL;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_STRING, key);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    dbkey = OcilibStringGetDBString(coll->sdb->env, key, &dbkeylen);

    OCI_CALL(oci_ret, OCISodaFindOneWithKey, coll->sdb->con->handle,
             coll->handle,
             (const OraText*)dbkey,
             (ub4)dbkeylen,
             (ub4)OCILIB_SODA_DOC_FLAGS(coll->sdb),
             &handle,
             coll->sdb->con->err,
             OCI_DEFAULT
             );

    if (OCI_NO_DATA == oci_ret)
    {
        OcilibErrorResetSource(NULL, doc);
        FREE(doc);
        doc = NULL;
        JUMP_CLEANUP();
    }

    if (OCI_FAILURE(oci_ret) && (OCI_NO_DATA != oci_ret))
    {
        THROW(OcilibExceptionOCI, coll->sdb->con->err, oci_ret);
    }

    doc = OcilibSodaDocumentInitialize(coll->sdb, NULL, handle, NULL, 0, NULL, 0);

    SET_RETVAL(doc);

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(coll->sdb->env, dbkey);

        if (FAILURE && NULL != doc)
        {
            OcilibMemoryFreeHandle(doc->sdb->env, (dvoid*)doc->handle, OCI_HTYPE_SODA_DOCUMENT);
            OcilibErrorResetSource(NULL, doc);
            FREE(doc);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionDocumentCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibSodaCollectionDocumentCount
(
    OCI_SodaCollection * coll,
    OCI_SodaOptions    * options
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt64, 0,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    OCI_SodaOptions        * tmpopts   = NULL;
    OCISodaOperationOptions* opthandle = NULL;
    ub8                      numdocs   = 0;
    OCI_UInt32               loops     = 1;
    OCI_UInt32               i         = 0;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    if (NULL != options)
    {
        opthandle = options->handle;

        /* multi-key emulation: sum per-key counts */

        if (options->key_count > 0)
        {
            loops = options->key_count;
        }

    }
    else
    {
        tmpopts = OcilibSodaOptionsCreate(coll->sdb);
        CHECK_NULL(tmpopts);

        opthandle = tmpopts->handle;
    }

    for (i = 0; i < loops; i++)
    {
        ub8 keydocs = 0;

        if (NULL != options && options->key_count > 0)
        {
            CHECK(OcilibSodaOptionsSetKeyAttr(options, options->keys[i]));
        }

        CHECK_OCI
        (
            coll->sdb->con->err,
            OCISodaDocCount,
            coll->sdb->con->handle,
            coll->handle,
            opthandle,
            &keydocs,
            coll->sdb->con->err,
            OCI_DEFAULT
        );

        numdocs += keydocs;
    }

    SET_RETVAL((OCI_UInt64)numdocs);

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != tmpopts)
        {
            OcilibSodaOptionsFree(tmpopts);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionDocumentCountWithFilter
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibSodaCollectionDocumentCountWithFilter
(
    OCI_SodaCollection* coll,
    const otext       * filterSpec
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt64, 0,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    dbtext  * dbflt    = NULL;
    OCI_Int32 dbfltlen = -1;
    ub8       numdocs  = 0;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_STRING, filterSpec);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    dbflt = OcilibStringGetDBString(coll->sdb->env, filterSpec, &dbfltlen);

    CHECK_OCI
    (
        coll->sdb->con->err,
        OCISodaDocCountWithFilter,
        coll->sdb->con->handle,
        coll->handle,
        (const OraText*)dbflt,
        (ub4)dbfltlen,
        &numdocs,
        coll->sdb->con->err,
        OCI_DEFAULT
    );

    SET_RETVAL((OCI_UInt64)numdocs);

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(coll->sdb->env, dbflt);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionReplaceOne
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionReplaceOne
(
    OCI_SodaCollection * coll,
    OCI_SodaOptions    * options,
    OCI_SodaDocument   * doc,
    OCI_Boolean          refresh
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    boolean                  replaced  = FALSE;
    OCI_SodaOptions        * tmpopts   = NULL;
    OCISodaOperationOptions* opthandle = NULL;
    OCI_UInt32               loops     = 1;
    OCI_UInt32               i         = 0;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_SODA_DOC, doc);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    if (NULL != options)
    {
        opthandle = options->handle;

        /* multi-key emulation: replace on the first matching key */

        if (options->key_count > 0)
        {
            loops = options->key_count;
        }

    }
    else
    {
        tmpopts = OcilibSodaOptionsCreate(coll->sdb);
        CHECK_NULL(tmpopts);

        opthandle = tmpopts->handle;
    }

    for (i = 0; i < loops && FALSE == replaced; i++)
    {

        if (NULL != options && options->key_count > 0)
        {
            CHECK(OcilibSodaOptionsSetKeyAttr(options, options->keys[i]));
        }

        if (refresh)
        {
            OCISodaDoc *reshandle = doc->handle;

            CHECK_OCI
            (
                coll->sdb->con->err,
                OCISodaReplOneAndGet,
                coll->sdb->con->handle,
                coll->handle,
                opthandle,
                &reshandle,
                &replaced,
                coll->sdb->con->err,
                OCI_DEFAULT
            );

            if (replaced)
            {
                CHECK(OcilibSodaDocumentRefreshMetadata(doc, reshandle));

                /* OCI does not populate the doc's key — the storage key comes
                   from the options, not the document */

                if (NULL != options && NULL != options->keys && NULL == doc->key)
                {
                    doc->key = OcilibStringDuplicate(coll->sdb->con->env, options->keys[i]);
                }
            }
        }
        else
        {
            CHECK_OCI
            (
                coll->sdb->con->err,
                OCISodaReplOne,
                coll->sdb->con->handle,
                coll->handle,
                opthandle,
                doc->handle,
                &replaced,
                coll->sdb->con->err,
                OCI_DEFAULT
            );
        }
    }

    SET_RETVAL((OCI_Boolean)replaced);

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != tmpopts)
        {
            OcilibSodaOptionsFree(tmpopts);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionReplaceOneByKey
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionReplaceOneByKey
(
    OCI_SodaCollection* coll,
    const otext       * key,
    OCI_SodaDocument  * doc,
    OCI_Boolean         refresh
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    boolean   replaced = FALSE;
    dbtext  * dbkey    = NULL;
    OCI_Int32 dbkeylen = -1;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_STRING, key);
    CHECK_PTR(OCI_IPC_SODA_DOC, doc);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    dbkey = OcilibStringGetDBString(coll->sdb->env, key, &dbkeylen);

    if (refresh)
    {
        OCISodaDoc *reshandle = doc->handle;

        CHECK_OCI
        (
            coll->sdb->con->err,
            OCISodaReplOneAndGetWithKey,
            coll->sdb->con->handle,
            coll->handle,
            (const OraText*)dbkey,
            (ub4)dbkeylen,
            &reshandle,
            &replaced,
            coll->sdb->con->err,
            OCI_DEFAULT
        );

        if (replaced)
        {
            CHECK(OcilibSodaDocumentRefreshMetadata(doc, reshandle));
        }
    }
    else
    {
        CHECK_OCI
        (
            coll->sdb->con->err,
            OCISodaReplOneWithKey,
            coll->sdb->con->handle,
            coll->handle,
            (const OraText*)dbkey,
            (ub4)dbkeylen,
            doc->handle,
            &replaced,
            coll->sdb->con->err,
            OCI_DEFAULT
        );
    }

    SET_RETVAL((OCI_Boolean)replaced);

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(coll->sdb->env, dbkey);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionRemove
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionRemove
(
    OCI_SodaCollection * coll,
    OCI_SodaOptions    * options,
    OCI_UInt64         * count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    ub8                      removeCount = 0;
    OCI_SodaOptions        * tmpopts     = NULL;
    OCISodaOperationOptions* opthandle   = NULL;
    OCI_UInt32               loops       = 1;
    OCI_UInt32               i           = 0;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    if (NULL != options)
    {
        opthandle = options->handle;

        /* multi-key emulation: remove per key and sum counts */

        if (options->key_count > 0)
        {
            loops = options->key_count;
        }

    }
    else
    {
        tmpopts = OcilibSodaOptionsCreate(coll->sdb);
        CHECK_NULL(tmpopts);

        opthandle = tmpopts->handle;
    }

    for (i = 0; i < loops; i++)
    {
        ub8 keyCount = 0;

        if (NULL != options && options->key_count > 0)
        {
            CHECK(OcilibSodaOptionsSetKeyAttr(options, options->keys[i]));
        }

        CHECK_OCI
        (
            coll->sdb->con->err,
            OCISodaRemove,
            coll->sdb->con->handle,
            coll->handle,
            opthandle,
            &keyCount,
            coll->sdb->con->err,
            OCI_DEFAULT
        );

        removeCount += keyCount;
    }

    if (NULL != count)
    {
        *count = (OCI_UInt64)removeCount;
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != tmpopts)
        {
            OcilibSodaOptionsFree(tmpopts);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionRemoveOneByKey
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionRemoveOneByKey
(
    OCI_SodaCollection* coll,
    const otext       * key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    boolean   removed  = FALSE;
    dbtext  * dbkey    = NULL;
    OCI_Int32 dbkeylen = -1;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_STRING, key);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    dbkey = OcilibStringGetDBString(coll->sdb->env, key, &dbkeylen);

    CHECK_OCI
    (
        coll->sdb->con->err,
        OCISodaRemoveOneWithKey,
        coll->sdb->con->handle,
        coll->handle,
        (const OraText*)dbkey,
        (ub4)dbkeylen,
        &removed,
        coll->sdb->con->err,
        OCI_DEFAULT
    );

    SET_RETVAL((OCI_Boolean)removed);

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(coll->sdb->env, dbkey);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionInsertMany
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionInsertMany
(
    OCI_SodaCollection * coll,
    OCI_SodaDocument  ** docs,
    OCI_UInt32           count,
    OCI_Boolean          refresh
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    OCISodaDoc          **handles = NULL;
    OCISodaOutputOptions *outopts = NULL;
    OCI_UInt32            i       = 0;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK(NULL != docs);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);
    CHECK(count > 0);

    handles = OcilibMemoryAlloc(coll->sdb->env, OCI_IPC_VOID, sizeof(*handles), count, TRUE);

    CHECK_NULL(handles);

    CHECK
    (
        OcilibMemoryAllocHandle
        (
            coll->sdb->env,
            (dvoid *)coll->sdb->env->handle,
            (dvoid **)&outopts,
            OCI_HTYPE_SODA_OUTPUT_OPTIONS
        )
    );

    for (i = 0; i < count; i++)
    {
        CHECK_PTR(OCI_IPC_SODA_DOC, docs[i]);
        handles[i] = docs[i]->handle;
    }

    if (refresh)
    {
        CHECK_OCI
        (
            coll->sdb->con->err,
            OCISodaBulkInsertAndGet,
            coll->sdb->con->handle,
            coll->handle,
            handles,
            (ub4)count,
            outopts,
            coll->sdb->con->err,
            OCI_DEFAULT
        );
    }
    else
    {
        CHECK_OCI
        (
            coll->sdb->con->err,
            OCISodaBulkInsert,
            coll->sdb->con->handle,
            coll->handle,
            handles,
            (ub4)count,
            outopts,
            coll->sdb->con->err,
            OCI_DEFAULT
        );
    }

    if (refresh)
    {
        for (i = 0; i < count; i++)
        {
            CHECK(OcilibSodaDocumentRefreshMetadata(docs[i], handles[i]));
        }
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != outopts)
        {
            OcilibMemoryFreeHandle(coll->sdb->env, (dvoid *)outopts, OCI_HTYPE_SODA_OUTPUT_OPTIONS);
        }
        if (NULL != handles)
        {
            OcilibMemoryFree(coll->sdb->env, handles);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionCreateIndex
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionCreateIndex
(
    OCI_SodaCollection * coll,
    const otext        * indexSpec
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    dbtext   *dbspec = NULL;
    OCI_Int32 dbsize = -1;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_STRING, indexSpec);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    dbspec = OcilibStringGetDBString(coll->sdb->env, indexSpec, &dbsize);

    CHECK_OCI
    (
        coll->sdb->con->err,
        OCISodaIndexCreate,
        coll->sdb->con->handle,
        coll->handle,
        (const OraText*)dbspec,
        (ub4)dbsize,
        coll->sdb->con->err,
        OCI_SODA_ATOMIC_COMMIT
    );

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(coll->sdb->env, dbspec);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionDropIndex
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionDropIndex
(
    OCI_SodaCollection * coll,
    const otext        * name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    dbtext   *dbkey    = NULL;
    OCI_Int32 dbkeylen = -1;
    boolean   dropped  = FALSE;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_STRING, name);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    dbkey = OcilibStringGetDBString(coll->sdb->env, name, &dbkeylen);

    CHECK_OCI
    (
        coll->sdb->con->err,
        OCISodaIndexDrop,
        coll->sdb->con->handle,
        (OraText*)dbkey,
        (ub4)dbkeylen,
        &dropped,
        coll->sdb->con->err,
        OCI_SODA_ATOMIC_COMMIT
    );

    SET_RETVAL((OCI_Boolean)dropped);

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(coll->sdb->env, dbkey);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionGetDataGuide
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDocument * OcilibSodaCollectionGetDataGuide
(
    OCI_SodaCollection * coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDocument*, NULL,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    OCI_SodaDocument *doc     = NULL;
    OCISodaDoc       *handle  = NULL;
    sword             oci_ret = 0;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    OCI_CALL(oci_ret, OCISodaDataGuideGet, coll->sdb->con->handle,
             coll->handle,
             0,
             &handle,
             coll->sdb->con->err,
             OCI_DEFAULT
             );

    if (OCI_NO_DATA == oci_ret)
    {
        OcilibErrorResetSource(NULL, coll);
        SET_RETVAL(NULL);
    }
    else if (OCI_FAILURE(oci_ret))
    {
        THROW(OcilibExceptionOCI, coll->sdb->con->err, oci_ret);
    }
    else
    {
        doc = OcilibSodaDocumentInitialize(coll->sdb, NULL, handle, NULL, 0, NULL, 0);

        SET_RETVAL(doc);
    }

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionSave
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionSave
(
    OCI_SodaCollection * coll,
    OCI_SodaDocument   * doc,
    OCI_Boolean          refresh
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_SODA_DOC, doc);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    CHECK_OCI
    (
        coll->sdb->con->err,
        OCISodaSave,
        coll->sdb->con->handle,
        coll->handle,
        doc->handle,
        coll->sdb->con->err,
        OCI_DEFAULT
    );

    if (refresh)
    {
        /* OCISodaSaveAndGet() crashes on some client/server combinations;
           emulate with a lookup to refresh the document metadata in place */

        const otext      *key   = OcilibSodaDocumentGetKey(doc);
        OCI_SodaDocument *found = NULL;

        CHECK(NULL != key);

        found = OcilibSodaCollectionFindOneByKey(coll, key);
        CHECK_NULL(found);

        {
            OCISodaDoc *fresh = found->handle;
            found->handle = NULL;    /* consumed by RefreshMetadata */

            CHECK(OcilibSodaDocumentRefreshMetadata(doc, fresh));
        }

        OcilibSodaDocumentFree(found);
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionListIndexes
 * --------------------------------------------------------------------------------------------- */

otext** OcilibSodaCollectionListIndexes
(
    OCI_SodaCollection * coll,
    OCI_UInt32         * count
)
{
    ENTER_FUNC
    (
        /* returns */ otext**, NULL,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    OCIColl   *list  = NULL;
    otext    **names = NULL;
    sb4        size  = 0;
    sb4        i     = 0;
    OCI_UInt32 found = 0;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_VOID, count);
    CHECK_PTR(OCI_IPC_SODA_DB, coll->sdb);
    CHECK_SODA_ENABLED(coll->sdb->con);

    *count = 0;

    CHECK_OCI
    (
        coll->sdb->con->err,
        OCISodaIndexList,
        coll->sdb->con->handle,
        coll->handle,
        (ub4)0,
        &list,
        coll->sdb->con->err,
        OCI_DEFAULT
    );

    CHECK_OCI
    (
        coll->sdb->con->err,
        OCICollSize,
        coll->sdb->env->handle,
        coll->sdb->con->err,
        list,
        &size
    );

    names = OcilibMemoryAlloc(coll->sdb->env, OCI_IPC_VOID, sizeof(*names),
                              (OCI_UInt32)(size > 0 ? size : 1), TRUE);

    CHECK_NULL(names);

    for (i = 0; i < size; i++)
    {
        dvoid     *elem    = NULL;
        dvoid     *elemind = NULL;
        boolean    exists  = FALSE;
        OCI_UInt32 bufsize = 0;

        CHECK_OCI
        (
            coll->sdb->con->err,
            OCICollGetElem,
            coll->sdb->env->handle,
            coll->sdb->con->err,
            list,
            i,
            &exists,
            &elem,
            &elemind
        );

        if (exists && NULL != elem)
        {
            /* elem is the element address — for this collection the elements
               are OCIString*, so *elem is the OCIString handle itself */
            names[found] = OcilibStringFromStringPtr(coll->sdb->env,
                                                     coll->sdb->env->handle,
                                                     *(OCIString **)elem,
                                                     &names[found],
                                                     &bufsize);
            CHECK_NULL(names[found]);
            found++;
        }
    }

    *count = found;

    SET_RETVAL(names);
    names = NULL;

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != list)
        {
            sword ret;

            OCI_CALL(ret, OCIObjectFree, coll->sdb->env->handle,
                     coll->sdb->con->err,
                     (dvoid *)list, (ub2)0);
        }
        if (NULL != names)
        {
            OCI_UInt32 j;
            for (j = 0; j < found; j++)
            {
                if (NULL != names[j])
                {
                    OcilibMemoryFree(coll->sdb->env, names[j]);
                }
            }
            OcilibMemoryFree(coll->sdb->env, names);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollectionFreeIndexNames
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollectionFreeIndexNames
(
    OCI_SodaCollection * coll,
    otext             ** names,
    OCI_UInt32           count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    OCI_UInt32 i = 0;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);

    if (NULL != names)
    {
        for (i = 0; i < count; i++)
        {
            if (NULL != names[i])
            {
                OcilibMemoryFree(coll->sdb->env, names[i]);
            }
        }

        OcilibMemoryFree(coll->sdb->env, names);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}
