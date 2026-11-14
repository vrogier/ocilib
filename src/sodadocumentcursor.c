/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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

#include "sodadocumentcursor.h"

#include "macros.h"
#include "memory.h"
#include "sodadocument.h"
#include "sodadatabase.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentCursorInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDocumentCursor * OcilibSodaDocumentCursorInitialize
(
    OCI_SodaDocumentCursor *cursor,
    OCI_SodaCollection     *coll,
    OCISodaDocCursor       *handle
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDocumentCursor*, NULL,
        /* context */ OCI_IPC_SODA_COLL, coll
    );

    OCI_Boolean alloc = FALSE;

    CHECK_PTR(OCI_IPC_SODA_COLL, coll);
    CHECK_PTR(OCI_IPC_VOID, handle);

    if (NULL == cursor)
    {
        ALLOC_HANDLE(OCI_IPC_SODA_DOC_CURSOR, cursor);
        alloc = TRUE;
    }

    cursor->coll   = coll;
    cursor->handle = handle;

    SET_RETVAL(cursor);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && alloc && NULL != cursor)
        {
            OcilibSodaDocumentCursorFree(cursor);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentCursorGetNext
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDocument * OcilibSodaDocumentCursorGetNext
(
    OCI_SodaDocumentCursor *cursor
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDocument*, NULL,
        /* context */ OCI_IPC_SODA_DOC_CURSOR, cursor
    );

    OCI_SodaDocument *doc     = NULL;
    sword             oci_ret = 0;

    CHECK_PTR(OCI_IPC_SODA_DOC_CURSOR, cursor);
    CHECK_PTR(OCI_IPC_SODA_COLL, cursor->coll);
    CHECK_PTR(OCI_IPC_SODA_DB, cursor->coll->sdb);
    CHECK_SODA_ENABLED(cursor->coll->sdb->con);

    ALLOC_HANDLE(OCI_IPC_SODA_DOC, doc);
    doc->sdb = cursor->coll->sdb;

    if (NULL == cursor->handle)
    {
        /* materialized results of an emulated multi-key find */

        if (cursor->doc_pos >= cursor->doc_count)
        {
            OcilibSodaDocumentFree(doc);
            doc = NULL;
            JUMP_CLEANUP();
        }

        doc->handle                   = cursor->docs[cursor->doc_pos];
        cursor->docs[cursor->doc_pos] = NULL;
        cursor->doc_pos++;
    }
    else
    {
        oci_ret = cursor->coll->sdb->env->api->OCISodaDocGetNext
                  (
                      cursor->coll->sdb->con->handle,
                      cursor->handle,
                      &doc->handle,
                      cursor->coll->sdb->con->err,
                      OCI_DEFAULT
                  );

        if (OCI_NO_DATA == oci_ret || NULL == doc->handle)
        {
            OcilibSodaDocumentFree(doc);
            doc = NULL;
            JUMP_CLEANUP();
        }

        CHECK(OCI_SUCCESS == oci_ret || OCI_SUCCESS_WITH_INFO == oci_ret);
    }

    CHECK(NULL != OcilibSodaDocumentInitialize(doc->sdb, doc, doc->handle, NULL, 0, NULL, 0));

    SET_RETVAL(doc);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != doc)
        {
            OcilibSodaDocumentFree(doc);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDocumentCursorFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaDocumentCursorFree
(
    OCI_SodaDocumentCursor *cursor
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_DOC_CURSOR, cursor
    );

    CHECK_PTR(OCI_IPC_SODA_DOC_CURSOR, cursor);

    if (NULL != cursor->handle)
    {
        OcilibMemoryFreeHandle(cursor->coll->sdb->env, (dvoid*)cursor->handle, OCI_HTYPE_SODA_DOC_CURSOR);
    }

    if (NULL != cursor->docs)
    {
        OCI_UInt32 i;

        for (i = 0; i < cursor->doc_count; i++)
        {
            if (NULL != cursor->docs[i])
            {
                OcilibMemoryFreeHandle(cursor->coll->sdb->env, (dvoid*)cursor->docs[i], OCI_HTYPE_SODA_DOCUMENT);
            }
        }

        OcilibMemoryFree(cursor->coll->sdb->env, cursor->docs);
    }

    OcilibErrorResetSource(NULL, cursor);
    FREE(cursor);

    SET_SUCCESS();

    EXIT_FUNC();
}
