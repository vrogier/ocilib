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
#include "strings.h"
#include "transaction.h"

/* --------------------------------------------------------------------------------------------- *
 * DatabaseStartup
 * --------------------------------------------------------------------------------------------- */

boolean DatabaseStartup
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
    OCI_Connection *con = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_REMOTE_DBS_CONTROL_ENABLED()

#if OCI_VERSION_COMPILE >= OCI_10_2

    if (start_mode & OCI_DB_SPM_START)
    {
        OCIAdmin *adm = NULL;

        /* connect with preliminary authentication mode */

        con = ConnectionCreate(db, user, pwd, sess_mode | OCI_PRELIM_AUTH);

        STATUS = (NULL != con);

        if (STATUS)
        {
            if (spfile && spfile[0])
            {
                dbtext *dbstr  = NULL;
                int     dbsize = -1;

                /* allocate administration handle */

                STATUS = MemoryAllocHandle((dvoid *)Env.env,  (dvoid **) (void *) &adm, OCI_HTYPE_ADMIN);

                /* set client file if provided */

                dbstr = StringGetDBString(spfile, &dbsize);

                ATTRIB_SET(OCI_HTYPE_ADMIN, OCI_ATTR_ADMIN_PFILE, adm, dbstr, dbsize)

                StringReleaseDBString(dbstr);
            }

            /* startup DB */

            EXEC(OCIDBStartup(con->cxt, con->err, (OCIAdmin *) adm, OCI_DEFAULT, start_flag))

            /* release security administration handle */

            if (adm)
            {
                MemoryFreeHandle(Env.err, OCI_HTYPE_ADMIN);
            }

            /* disconnect */

            ConnectionFree(con);
        }
    }

    if (STATUS)
    {
        /* connect without preliminary mode */

        con = ConnectionCreate(db, user, pwd, sess_mode);

        STATUS = (NULL != con);

        /* alter database */

        if (STATUS)
        {
            OCI_Statement *stmt = StatementCreate(con);

            /* mount database */

            if (start_mode & OCI_DB_SPM_MOUNT)
            {
                STATUS = STATUS && StatementExecuteStmt(stmt, OTEXT("ALTER DATABASE MOUNT"));
            }

            /* open database */

            if (start_mode & OCI_DB_SPM_OPEN)
            {
                STATUS = STATUS && StatementExecuteStmt(stmt, OTEXT("ALTER DATABASE OPEN"));
            }

            StatementFree(stmt);

            /* disconnect */

            ConnectionFree(con);
        }
    }

#else

    OCI_NOT_USED(db)
    OCI_NOT_USED(user)
    OCI_NOT_USED(pwd)
    OCI_NOT_USED(sess_mode)
    OCI_NOT_USED(start_mode)
    OCI_NOT_USED(start_flag)
    OCI_NOT_USED(spfile)
    OCI_NOT_USED(con)

#endif

    call_retval = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DatabaseShutdown
 * --------------------------------------------------------------------------------------------- */

boolean DatabaseShutdown
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int sess_mode,
    unsigned int shut_mode,
    unsigned int shut_flag
)
{
    OCI_Connection *con = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_REMOTE_DBS_CONTROL_ENABLED()

#if OCI_VERSION_COMPILE >= OCI_10_2

    /* connect to server */

    con = ConnectionCreate(db, user, pwd, sess_mode);

    STATUS = (NULL != con);

    if (STATUS)
    {
        /* delete current transaction before the abort */

        if (con->trs && (OCI_DB_SDF_ABORT == shut_flag))
        {
            TransactionFree(con->trs);

            con->trs = NULL;
        }

        /* start shutdown */

        if (shut_mode & OCI_DB_SDM_SHUTDOWN)
        {
            /* start shutdown process */

            EXEC(OCIDBShutdown(con->cxt, con->err, (OCIAdmin *) NULL, shut_flag))
        }

        /* alter database if we are not in abort mode */

        if (STATUS && (OCI_DB_SDF_ABORT != shut_flag))
        {
            OCI_Statement *stmt = StatementCreate(con);

            /* close database */

            if (shut_mode & OCI_DB_SDM_CLOSE)
            {
                STATUS = STATUS && StatementExecuteStmt(stmt, OTEXT("ALTER DATABASE CLOSE NORMAL"));
            }

            /* unmount database */

            if (shut_mode & OCI_DB_SDM_DISMOUNT)
            {
                STATUS = STATUS && StatementExecuteStmt(stmt, OTEXT("ALTER DATABASE DISMOUNT"));
            }

            StatementFree(stmt);

            /* delete current transaction before the shutdown */

            if (con->trs)
            {
                TransactionFree(con->trs);

                con->trs = NULL;
            }

            /* do the final shutdown if we are not in abort mode */

            EXEC(OCIDBShutdown(con->cxt, con->err, (OCIAdmin *) 0, OCI_DBSHUTDOWN_FINAL))
        }

        /* disconnect */

        ConnectionFree(con);
    }

#else

    OCI_NOT_USED(db)
    OCI_NOT_USED(user)
    OCI_NOT_USED(pwd)
    OCI_NOT_USED(sess_mode)
    OCI_NOT_USED(shut_mode)
    OCI_NOT_USED(shut_flag)
    OCI_NOT_USED(con)

#endif

    call_retval = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DatabaseSetUserPassword
 * --------------------------------------------------------------------------------------------- */

boolean DatabaseSetUserPassword
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    const otext* new_pwd
)
{
    OCI_Connection* con = NULL;

    CALL_ENTER(boolean, FALSE)

    /* let's be sure OCI_Initialize() has been called */

    CALL_CHECK_INITIALIZED()
    CALL_CHECK_PTR(OCI_IPC_STRING, pwd)
    CALL_CHECK_PTR(OCI_IPC_STRING, new_pwd)

    con = ConnectionAllocate(NULL, db, user, pwd, OCI_AUTH);

    if (con)
    {
        if (!ConnectionAttach(con) || !ConnectionLogon(con, new_pwd, NULL))
        {
            ConnectionFree(con);
            con = NULL;
        }
    }

    STATUS = (con != NULL);

    if (STATUS)
    {
        ConnectionFree(con);
    }

    RETVAL = STATUS;

    CALL_EXIT()
}