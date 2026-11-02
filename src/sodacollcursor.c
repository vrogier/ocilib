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

#include "sodacollcursor.h"

#include "macros.h"
#include "memory.h"
#include "sodacoll.h"
#include "sodadb.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollCursorInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_SodaCollCursor * OcilibSodaCollCursorInitialize
(
    OCI_SodaCollCursor *cursor,
    OCI_SodaDb         *sdb,
    OCISodaCollCursor  *handle
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaCollCursor*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCI_Boolean alloc = FALSE;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_PTR(OCI_IPC_VOID, handle);

    if (NULL == cursor)
    {
        ALLOC_HANDLE(OCI_IPC_SODA_COLL_CURSOR, cursor);
        alloc = TRUE;
    }

    cursor->sdb    = sdb;
    cursor->handle = handle;

    SET_RETVAL(cursor);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && alloc && NULL != cursor)
        {
            OcilibMemoryFreeHandle(sdb->env, (dvoid*)cursor->handle, OCI_HTYPE_SODA_COLL_CURSOR);
            OcilibErrorResetSource(NULL, cursor);
            FREE(cursor);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollCursorGetNext
 * --------------------------------------------------------------------------------------------- */

OCI_SodaColl * OcilibSodaCollCursorGetNext
(
    OCI_SodaCollCursor *cursor
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaColl*, NULL,
        /* context */ OCI_IPC_SODA_COLL_CURSOR, cursor
    );

    OCI_SodaColl *coll    = NULL;
    sword         oci_ret = 0;

    CHECK_PTR(OCI_IPC_SODA_COLL_CURSOR, cursor);
    CHECK_PTR(OCI_IPC_SODA_DB, cursor->sdb);
    CHECK_SODA_ENABLED(cursor->sdb->con);

    ALLOC_HANDLE(OCI_IPC_SODA_COLL, coll);
    coll->sdb = cursor->sdb;

    oci_ret = cursor->sdb->env->api->OCISodaCollGetNext
              (
                  cursor->sdb->con->handle,
                  cursor->handle,
                  &coll->handle,
                  cursor->sdb->con->err,
                  OCI_DEFAULT
              );

    if (OCI_NO_DATA == oci_ret || NULL == coll->handle)
    {
        OcilibErrorResetSource(NULL, coll);
        FREE(coll);
        coll = NULL;
        JUMP_CLEANUP();
    }

    CHECK(OCI_SUCCESS == oci_ret || OCI_SUCCESS_WITH_INFO == oci_ret);

    CHECK(NULL != OcilibSodaCollInitialize(coll, cursor->sdb, coll->handle));

    SET_RETVAL(coll);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != coll)
        {
            OcilibMemoryFreeHandle(coll->sdb->env, (dvoid*)coll->handle, OCI_HTYPE_SODA_COLLECTION);
            OcilibErrorResetSource(NULL, coll);
            FREE(coll);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaCollCursorFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaCollCursorFree
(
    OCI_SodaCollCursor *cursor
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_COLL_CURSOR, cursor
    );

    CHECK_PTR(OCI_IPC_SODA_COLL_CURSOR, cursor);

    if (NULL != cursor->handle)
    {
        OcilibMemoryFreeHandle(cursor->sdb->env, (dvoid*)cursor->handle, OCI_HTYPE_SODA_COLL_CURSOR);
    }

    OcilibErrorResetSource(NULL, cursor);
    FREE(cursor);

    SET_SUCCESS();

    EXIT_FUNC();
}
