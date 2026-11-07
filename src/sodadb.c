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

#include "sodadb.h"

#include "macros.h"
#include "memory.h"
#include "sodacoll.h"
#include "sodacollcursor.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDbCreate
 * --------------------------------------------------------------------------------------------- */

OCI_SodaDb * OcilibSodaDbCreate
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaDb*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_SodaDb *sdb = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_SODA_ENABLED(con);

    CHECK_NULL(con->env->api);
    CHECK_NULL(con->env->api->OCISodaCollOpen);
    CHECK_NULL(con->env->api->OCISodaDocCreate);
    CHECK_NULL(con->env->api->OCISodaFind);
    CHECK_NULL(con->env->api->OCISodaDocGetNext);

    ALLOC_HANDLE(OCI_IPC_SODA_DB, sdb);
    sdb->con = con;

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibSodaDbFree(sdb);
            sdb = NULL;
        }

        SET_RETVAL(sdb);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDbFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaDbFree
(
    OCI_SodaDb *sdb
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
 * OcilibSodaDbGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibSodaDbGetConnection
(
    OCI_SodaDb *sdb
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
 * OcilibSodaDbCreateColl
 * --------------------------------------------------------------------------------------------- */

OCI_SodaColl * OcilibSodaDbCreateColl
(
    OCI_SodaDb  *sdb,
    const otext *name,
    const otext *metadata
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaColl*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCI_SodaColl *coll     = NULL;
    dbtext       *dbstr    = NULL;
    dbtext       *dbmta    = NULL;
    OCI_Int32     dbsize   = -1;
    OCI_Int32     dbmtalen = -1;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_PTR(OCI_IPC_STRING, name);
    CHECK_SODA_ENABLED(sdb->con);

    ALLOC_HANDLE(OCI_IPC_SODA_COLL, coll);
    coll->sdb = sdb;

    dbstr = OcilibStringGetDBString(sdb->env, name, &dbsize);
    dbmta = OcilibStringGetDBString(sdb->env, metadata, &dbmtalen);

    CHECK_OCI
    (
        sdb->con->err,
        OCISodaCollCreateWithMetadata,
        sdb->con->handle,
        (const OraText*)dbstr,
        (ub4)dbsize,
        (OraText*)dbmta,
        (ub4)dbmtalen,
        &coll->handle,
        sdb->con->err,
        (ub4)OCI_DEFAULT
    );

    CHECK(NULL != OcilibSodaCollInitialize(coll, sdb, coll->handle));

    SET_RETVAL(coll);

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(sdb->env, dbstr);
        OcilibStringReleaseDBString(sdb->env, dbmta);

        if (FAILURE && NULL != coll)
        {
            OcilibMemoryFreeHandle(sdb->env, (dvoid*)coll->handle, OCI_HTYPE_SODA_COLLECTION);
            OcilibErrorResetSource(NULL, coll);
            FREE(coll);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDbOpenColl
 * --------------------------------------------------------------------------------------------- */

OCI_SodaColl * OcilibSodaDbOpenColl
(
    OCI_SodaDb  *sdb,
    const otext *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaColl*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCI_SodaColl *coll   = NULL;
    dbtext       *dbstr  = NULL;
    OCI_Int32     dbsize = -1;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_PTR(OCI_IPC_STRING, name);
    CHECK_SODA_ENABLED(sdb->con);

    ALLOC_HANDLE(OCI_IPC_SODA_COLL, coll);
    coll->sdb = sdb;

    dbstr = OcilibStringGetDBString(sdb->env, name, &dbsize);

    {
        sword oci_ret = sdb->con->env->api->OCISodaCollOpen
                        (
                            sdb->con->handle,
                            (const OraText*)dbstr,
                            (ub4)dbsize,
                            &coll->handle,
                            sdb->con->err,
                            OCI_DEFAULT
                        );

        CHECK
        (
            OCI_SUCCESS == oci_ret || OCI_SUCCESS_WITH_INFO == oci_ret || OCI_NO_DATA == oci_ret
        );
    }

    if (NULL == coll->handle)
    {
        OcilibErrorResetSource(NULL, coll);
        FREE(coll);
        SET_RETVAL(NULL);
    }
    else
    {
        CHECK(NULL != OcilibSodaCollInitialize(coll, sdb, coll->handle));
        SET_RETVAL(coll);
    }

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(sdb->env, dbstr);

        if (FAILURE && NULL != coll)
        {
            OcilibMemoryFreeHandle(sdb->env, (dvoid*)coll->handle, OCI_HTYPE_SODA_COLLECTION);
            OcilibErrorResetSource(NULL, coll);
            FREE(coll);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDbGetColls
 * --------------------------------------------------------------------------------------------- */

OCI_SodaCollCursor * OcilibSodaDbGetColls
(
    OCI_SodaDb  *sdb,
    const otext *startName
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_SodaCollCursor*, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCI_SodaCollCursor *cursor = NULL;
    dbtext             *dbstr  = NULL;
    OCI_Int32           dbsize = 0;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_SODA_ENABLED(sdb->con);

    ALLOC_HANDLE(OCI_IPC_SODA_COLL_CURSOR, cursor);
    cursor->sdb = sdb;

    if (NULL != startName)
    {
        dbsize = -1;
        dbstr  = OcilibStringGetDBString(sdb->env, startName, &dbsize);
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

    CHECK(NULL != OcilibSodaCollCursorInitialize(cursor, sdb, cursor->handle));

    SET_RETVAL(cursor);

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(sdb->env, dbstr);

        if (FAILURE && NULL != cursor)
        {
            OcilibMemoryFreeHandle(sdb->env, (dvoid*)cursor->handle, OCI_HTYPE_SODA_COLL_CURSOR);
            OcilibErrorResetSource(NULL, cursor);
            FREE(cursor);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibSodaDbGetCollectionNames
 * --------------------------------------------------------------------------------------------- */

otext** OcilibSodaDbGetCollectionNames
(
    OCI_SodaDb * sdb,
    OCI_UInt32 * count
)
{
    ENTER_FUNC
    (
        /* returns */ otext**, NULL,
        /* context */ OCI_IPC_SODA_DB, sdb
    );

    OCI_SodaCollCursor *cursor = NULL;
    OCI_SodaColl       *coll   = NULL;
    otext             **names  = NULL;
    OCI_UInt32          cap    = 8;
    OCI_UInt32          found  = 0;

    CHECK_PTR(OCI_IPC_SODA_DB, sdb);
    CHECK_PTR(OCI_IPC_VOID, count);
    CHECK_SODA_ENABLED(sdb->con);

    *count = 0;

    names = OcilibMemoryAlloc(sdb->env, OCI_IPC_VOID, sizeof(*names), cap, TRUE);

    CHECK_NULL(names);

    cursor = OcilibSodaDbGetColls(sdb, NULL);

    CHECK_NULL(cursor);

    while (NULL != (coll = OcilibSodaCollCursorGetNext(cursor)))
    {
        const otext *name = OcilibSodaCollGetName(coll);

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

        OcilibSodaCollFree(coll);
        coll = NULL;
    }

    CHECK(OcilibSodaCollCursorFree(cursor));
    cursor = NULL;

    *count = found;

    SET_RETVAL(names);
    names = NULL;

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != coll)
        {
            OcilibSodaCollFree(coll);
        }
        if (NULL != cursor)
        {
            OcilibSodaCollCursorFree(cursor);
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
 * OcilibSodaDbFreeCollectionNames
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibSodaDbFreeCollectionNames
(
    OCI_SodaDb * sdb,
    otext     ** names,
    OCI_UInt32   count
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
