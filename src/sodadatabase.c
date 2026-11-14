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

#include "sodadatabase.h"

#include "macros.h"
#include "memory.h"
#include "sodacollection.h"
#include "sodacollectioncursor.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDatabaseCreate
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDatabase * OcilibSodaDatabaseCreate
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDatabase*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_SodaDatabase *sdb = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_SODA_ENABLED(con);

    ALLOC_HANDLE(OCI_IPC_SODA_DB, sdb);
    sdb->con = con;

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibSodaDatabaseFree(sdb);
            sdb = NULL;
        }

        SET_RETVAL(sdb);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDatabaseFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaDatabaseFree
(
    OCI_SodaDatabase *sdb
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);

    OcilibErrorResetSource(NULL, sdb);

    FREE(sdb);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDatabaseGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibSodaDatabaseGetConnection
(
    OCI_SodaDatabase *sdb
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Connection*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_PTR(OCI_IPC_CONNECTION, sdb->con);

    SET_RETVAL(sdb->con);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDatabaseCreateCollection
 * --------------------------------------------------------------------------------------------- */

OCI_SodaCollection * OcilibSodaDatabaseCreateCollection
(
    OCI_SodaDatabase *sdb,
    const otext      *name,
    const otext      *metadata
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaCollection*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCI_SodaCollection *coll = NULL;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_PTR(OCI_IPC_STRING, name);
    CHECK_SODA_ENABLED(sdb->con);

    ALLOC_HANDLE(OCI_IPC_SODA_COLL, coll);
    coll->sdb = sdb;

    CHECK_OCI
    (
        sdb->con->err,
        OCISodaCollCreateWithMetadata,
        sdb->con->handle,
        (const OraText*)(name),
        (ub4)OcilibStringDBSize(name),
        (OraText*)(metadata),
        (ub4)OcilibStringDBSize(metadata),
        &coll->handle,
        sdb->con->err,
        (ub4)OCI_DEFAULT
    );

    CHECK(NULL != OcilibSodaCollectionInitialize(coll, sdb, coll->handle));

    SET_RETVAL(coll);

    CLEANUP_AND_EXIT_FUNC
    (

        if (FAILURE && NULL != coll)
        {
            OcilibSodaCollectionFree(coll);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDatabaseOpenCollection
 * --------------------------------------------------------------------------------------------- */

OCI_SodaCollection * OcilibSodaDatabaseOpenCollection
(
    OCI_SodaDatabase *sdb,
    const otext      *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaCollection*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCI_SodaCollection *coll = NULL;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_PTR(OCI_IPC_STRING, name);
    CHECK_SODA_ENABLED(sdb->con);

    ALLOC_HANDLE(OCI_IPC_SODA_COLL, coll);
    coll->sdb = sdb;

    sword oci_ret = sdb->con->env->api->OCISodaCollOpen
                    (
                        sdb->con->handle,
                        (const OraText*)(name),
                        (ub4)OcilibStringDBSize(name),
                        &coll->handle,
                        sdb->con->err,
                        OCI_DEFAULT
                    );

    CHECK
    (
        OCI_SUCCESS == oci_ret || OCI_SUCCESS_WITH_INFO == oci_ret || OCI_NO_DATA == oci_ret
    );

    if (NULL == coll->handle)
    {
        OcilibSodaCollectionFree(coll);
        SET_RETVAL(NULL);
    }
    else
    {
        CHECK(NULL != OcilibSodaCollectionInitialize(coll, sdb, coll->handle));
        SET_RETVAL(coll);
    }

    CLEANUP_AND_EXIT_FUNC
    (

        if (FAILURE && NULL != coll)
        {
            OcilibSodaCollectionFree(coll);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDatabaseGetCollections
 * --------------------------------------------------------------------------------------------- */

OCI_SodaCollectionCursor * OcilibSodaDatabaseGetCollections
(
    OCI_SodaDatabase *sdb,
    const otext      *startName
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaCollectionCursor*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCI_SodaCollectionCursor *cursor = NULL;
    dbtext                   *dbstr  = NULL;
    OCI_Int32                 dbsize = 0;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_SODA_ENABLED(sdb->con);

    ALLOC_HANDLE(OCI_IPC_SODA_COLL_CURSOR, cursor);
    cursor->sdb = sdb;

    if (NULL != startName)
    {
        dbstr  = (dbtext *) (startName);
        dbsize = OcilibStringDBSize(startName);
    }

    CHECK_OCI
    (
        sdb->con->err,
        OCISodaCollList,
        sdb->con->handle,
        (const OraText*)dbstr,
        (ub4)dbsize,
        &cursor->handle,
        sdb->con->err,
        OCI_DEFAULT
    );

    CHECK(NULL != OcilibSodaCollectionCursorInitialize(cursor, sdb, cursor->handle));

    SET_RETVAL(cursor);

    CLEANUP_AND_EXIT_FUNC
    (

        if (FAILURE && NULL != cursor)
        {
            OcilibSodaCollectionCursorFree(cursor);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDatabaseGetCollectionNames
 * --------------------------------------------------------------------------------------------- */

otext** OcilibSodaDatabaseGetCollectionNames
(
    OCI_SodaDatabase * sdb,
    OCI_UInt32       * count
)
{
    ENTER_FUNC
    (
        /* returns */ otext**, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCI_SodaCollectionCursor *cursor = NULL;
    OCI_SodaCollection       *coll   = NULL;
    otext                   **names  = NULL;
    OCI_UInt32                cap    = 8;
    OCI_UInt32                found  = 0;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_PTR(OCI_IPC_VOID, count);
    CHECK_SODA_ENABLED(sdb->con);

    *count = 0;

    names = OcilibMemoryAlloc(sdb->env, OCI_IPC_VOID, sizeof(*names), cap, TRUE);

    CHECK_NULL(names);

    cursor = OcilibSodaDatabaseGetCollections(sdb, NULL);

    CHECK_NULL(cursor);

    while (NULL != (coll = OcilibSodaCollectionCursorGetNext(cursor)))
    {
        const otext *name = OcilibSodaCollectionGetName(coll);

        if (NULL != name)
        {
            if (found == cap)
            {
                otext **grown = OcilibMemoryAlloc(sdb->env, OCI_IPC_VOID,
                                                  sizeof(*grown), cap * 2, TRUE);
                CHECK_NULL(grown);

                memcpy(grown, names, cap * sizeof(*names));
                OcilibMemoryFree(sdb->env, names);

                names = grown;
                cap  *= 2;
            }

            names[found] = OcilibStringDuplicate(sdb->env, name);
            CHECK_NULL(names[found]);
            found++;
        }

        OcilibSodaCollectionFree(coll);
        coll = NULL;
    }

    CHECK(OcilibSodaCollectionCursorFree(cursor));
    cursor = NULL;

    *count = found;

    SET_RETVAL(names);
    names = NULL;

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != coll)
        {
            OcilibSodaCollectionFree(coll);
        }
        if (NULL != cursor)
        {
            OcilibSodaCollectionCursorFree(cursor);
        }
        if (NULL != names)
        {
            OCI_UInt32 j;
            for (j = 0; j < found; j++)
            {
                if (NULL != names[j])
                {
                    OcilibMemoryFree(sdb->env, names[j]);
                }
            }
            OcilibMemoryFree(sdb->env, names);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDatabaseFreeCollectionNames
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaDatabaseFreeCollectionNames
(
    OCI_SodaDatabase * sdb,
    otext           ** names,
    OCI_UInt32         count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCI_UInt32 i = 0;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);

    if (NULL != names)
    {
        for (i = 0; i < count; i++)
        {
            if (NULL != names[i])
            {
                OcilibMemoryFree(sdb->env, names[i]);
            }
        }

        OcilibMemoryFree(sdb->env, names);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}
