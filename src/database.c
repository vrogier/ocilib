/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2020 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "database.h"

#include "array.h"
#include "connection.h"
#include "error.h"
#include "macros.h"
#include "statement.h"
#include "stringutils.h"
#include "transaction.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibDatabaseStartup
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDatabaseStartup
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int sess_mode,
    unsigned int start_mode,
    unsigned int start_flag,
    const otext *spfile
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    OCI_Connection *con = NULL;
    OCI_Statement *stmt = NULL;

#if OCI_VERSION_COMPILE >= OCI_10_2

    OCIAdmin* adm = NULL;

    dbtext* dbstr  = NULL;
    int     dbsize = -1;

#endif

    CHECK_REMOTE_DBS_CONTROL_ENABLED()

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (start_mode & OCI_DB_SPM_START)
    {
        /* connect with preliminary authentication mode */

        con = OcilibConnectionCreate(db, user, pwd, sess_mode | OCI_PRELIM_AUTH);
        CHECK_NULL(con)

        if (IS_STRING_VALID(spfile))
        {
            /* allocate administration handle */

            CHECK(OcilibMemoryAllocHandle((dvoid *)Env.env,  (dvoid **) (void *) &adm, OCI_HTYPE_ADMIN))

            /* set client file if provided */

            dbstr = OcilibStringGetDBString(spfile, &dbsize);

            CHECK_ATTRIB_SET
            (
                OCI_HTYPE_ADMIN, OCI_ATTR_ADMIN_PFILE,
                adm, dbstr, dbsize,
                Env.err
            )
        }

        /* startup DB */

        CHECK_OCI
        (
            Env.err,
            OCIDBStartup,
            con->cxt, con->err,
            (OCIAdmin *) adm, OCI_DEFAULT,
            start_flag
        )

        OcilibConnectionFree(con);
    }

    /* connect without preliminary mode */

    con = OcilibConnectionCreate(db, user, pwd, sess_mode);
    CHECK_NULL(con)

    /* alter OcilibDatabase */

    stmt = OcilibStatementCreate(con);
    CHECK_NULL(stmt)

    /* mount OcilibDatabase */

    if (start_mode & OCI_DB_SPM_MOUNT)
    {
        CHECK(OcilibStatementExecuteStmt(stmt, OTEXT("ALTER OcilibDatabase MOUNT")))
    }

    /* open OcilibDatabase */

    if (start_mode & OCI_DB_SPM_OPEN)
    {
        CHECK(OcilibStatementExecuteStmt(stmt, OTEXT("ALTER OcilibDatabase OPEN")))
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr);

        if (NULL != stmt)
        {
            OcilibStatementFree(stmt);
        }

        if (NULL != con)
        {
            OcilibConnectionFree(con);
        }

        if (NULL != adm)
        {
            OcilibMemoryFreeHandle(Env.err, OCI_HTYPE_ADMIN);
        }
    )

#else

    OCI_NOT_USED(db)
    OCI_NOT_USED(user)
    OCI_NOT_USED(pwd)
    OCI_NOT_USED(sess_mode)
    OCI_NOT_USED(start_mode)
    OCI_NOT_USED(start_flag)
    OCI_NOT_USED(spfile)
    OCI_NOT_USED(con)
    OCI_NOT_USED(stmt)

    CHECK(FALSE)

    EXIT_FUNC()

#endif

}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDatabaseShutdown
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDatabaseShutdown
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int sess_mode,
    unsigned int shut_mode,
    unsigned int shut_flag
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    OCI_Connection *con = NULL;
    OCI_Statement *stmt = NULL;

    CHECK_REMOTE_DBS_CONTROL_ENABLED()

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* connect to server */

    con = OcilibConnectionCreate(db, user, pwd, sess_mode);
    CHECK_NULL(con)

    /* delete current transaction before the abort */

    if (NULL != con->trs && (OCI_DB_SDF_ABORT == shut_flag))
    {
        OcilibTransactionFree(con->trs);
        con->trs = NULL;
    }

    /* start shutdown */

    if (shut_mode & OCI_DB_SDM_SHUTDOWN)
    {
        /* start shutdown process */

        CHECK_OCI
        (
            Env.err,
            OCIDBShutdown,
            con->cxt, con->err,
            (OCIAdmin *) NULL,
            shut_flag
        )
    }

    /* alter OcilibDatabase if we are not in abort mode */

    if (OCI_DB_SDF_ABORT != shut_flag)
    {
        stmt = OcilibStatementCreate(con);
        CHECK_NULL(stmt)

        /* close OcilibDatabase */

        if (shut_mode & OCI_DB_SDM_CLOSE)
        {
            CHECK(OcilibStatementExecuteStmt(stmt, OTEXT("ALTER OcilibDatabase CLOSE NORMAL")))
        }

        /* unmount OcilibDatabase */

        if (shut_mode & OCI_DB_SDM_DISMOUNT)
        {
            CHECK(OcilibStatementExecuteStmt(stmt, OTEXT("ALTER OcilibDatabase DISMOUNT")))
        }

        OcilibStatementFree(stmt);
        stmt = NULL;

        /* delete current transaction before the shutdown */

        if (NULL != con->trs)
        {
            OcilibTransactionFree(con->trs);
            con->trs = NULL;
        }

        /* do the final shutdown if we are not in abort mode */

        CHECK_OCI
        (
            Env.err,
            OCIDBShutdown,
            con->cxt, con->err,
            (OCIAdmin *) 0,
            OCI_DBSHUTDOWN_FINAL
        )

        OcilibConnectionFree(con);
        con = NULL;
    }

    SET_SUCCESS()

#else

    OCI_NOT_USED(db)
    OCI_NOT_USED(user)
    OCI_NOT_USED(pwd)
    OCI_NOT_USED(sess_mode)
    OCI_NOT_USED(shut_mode)
    OCI_NOT_USED(shut_flag)
    OCI_NOT_USED(con)

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != stmt)
        {
            OcilibStatementFree(stmt);
        }

        if (NULL != con)
        {
            OcilibConnectionFree(con);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDatabaseSetUserPassword
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDatabaseSetUserPassword
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    const otext* new_pwd
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    OCI_Connection* con = NULL;

    /* let's be sure OCI_Initialize() has been called */

    CHECK_INITIALIZED()
    CHECK_PTR(OCI_IPC_STRING, pwd)
    CHECK_PTR(OCI_IPC_STRING, new_pwd)

    /* create a connection in auth mode */

    con = OcilibConnectionAllocate( NULL, db, user, pwd, OCI_AUTH);
    CHECK_NULL(con)

    /* ConnectionLogon() will reset the password  */

    CHECK(OcilibConnectionAttach(con))
    CHECK(OcilibConnectionLogon(con, new_pwd, NULL))

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != con)
        {
            OcilibConnectionFree(con);
        }
    )
}