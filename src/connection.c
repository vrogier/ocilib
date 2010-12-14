/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2010 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: connection.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionAllocate
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_ConnectionAllocate
(
    OCI_Pool    *pool,
    const mtext *db,
    const mtext *user,
    const mtext *pwd,
    unsigned int mode
)
{
    OCI_Connection *con = NULL;
    OCI_List *list      = NULL;
    OCI_Item *item      = NULL;
    boolean res         = TRUE;

    /* create connection object */

    if (pool != NULL)
        list = pool->cons;
    else
        list = OCILib.cons;

    item = OCI_ListAppend(list, sizeof(*con));

    if (item != NULL)
    {
        con = (OCI_Connection *) item->data;

        /* create internal lists */

        con->stmts = OCI_ListCreate(OCI_IPC_STATEMENT);

        if (res == TRUE)
        {
            con->tinfs = OCI_ListCreate(OCI_IPC_TYPE_INFO);
            res        = (con->tinfs != NULL);
        }

        if (res == TRUE)
        {
            con->trsns = OCI_ListCreate(OCI_IPC_TRANSACTION);
            res        = (con->trsns != NULL);
        }

        /* set attributes */

        if (res == TRUE)
        {
            con->mode     = mode;
            con->pool     = pool;
            con->sess_tag = NULL;

            if (con->pool != NULL)
            {
                con->db   = (mtext *) db;
                con->user = (mtext *) user;
                con->pwd  = (mtext *) pwd;
            }
            else
            {
                con->db   = mtsdup(db   != NULL ? db   : MT(""));
                con->user = mtsdup(user != NULL ? user : MT(""));
                con->pwd  = mtsdup(pwd  != NULL ? pwd  : MT(""));
            }
        }

        /*  allocate error handle */

        if (res == TRUE)
            res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env,
                                                  (dvoid **) (void *) &con->err,
                                                  (ub4) OCI_HTYPE_ERROR,
                                                  (size_t) 0, (dvoid **) NULL));
    }
    else
        res = FALSE;

    /* update internal status */

    if (res == TRUE)
    {
        con->cstate = OCI_CONN_ALLOCATED;
    }
    else
    {
        OCI_ConnectionFree(con);
        con = NULL;
    }

    return con;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionDeallocate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionDeallocate
(
    OCI_Connection *con
)
{
    OCI_CHECK(con == NULL, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE);

    /* close error handle */

    if (con->err != NULL)
        OCI_HandleFree((dvoid *) con->err, (ub4) OCI_HTYPE_ERROR);

    /* close server handle in case of login error */

    if (con->svr != NULL)
        OCI_HandleFree((dvoid *) con->svr, (ub4) OCI_HTYPE_SERVER);

    con->cxt = NULL;
    con->ses = NULL;
    con->svr = NULL;
    con->err = NULL;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionAttach
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionAttach
(
    OCI_Connection *con
)
{
    void *ostr  = NULL;
    int osize   = -1;
    boolean res = TRUE;
    ub4 cmode   = OCI_DEFAULT;

    OCI_CHECK(con == NULL, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE);

    /* allocate server handle for non pooled conenction */

    if (con->pool == NULL)
    {
        res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env,
                                              (dvoid **) (void *) &con->svr,
                                              (ub4) OCI_HTYPE_SERVER,
                                              (size_t) 0, (dvoid **) NULL));

        /* attach server handle to service name */

        #if OCI_VERSION_COMPILE >= OCI_9_0

        if (OCILib.version_runtime >= OCI_9_0 && con->pool != NULL)
        {
            ostr  = OCI_GetInputMetaString(con->pool->name, &osize);
            cmode = OCI_CPOOL;
        }
        else

        #endif

        {
            ostr = OCI_GetInputMetaString(con->db, &osize);
        }

        OCI_CALL2
        (
            res, con,

            OCIServerAttach(con->svr, con->err,(OraText *) ostr,
                            (sb4) osize, cmode)
        )

        OCI_ReleaseMetaString(ostr);
    }

    /* handle errors */

    if (res == TRUE)
    {
        if (OCILib.version_runtime < OCI_9_0 && con->pool != NULL)
            con->pool->nb_opened++;

        con->cstate = OCI_CONN_ATTACHED;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionDetach
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionDetach
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK(con == NULL, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_ATTACHED, FALSE);

    if (con->svr != NULL)
    {
        /* detach from the oracle server */

        OCI_CALL2
        (
            res, con,

            OCIServerDetach(con->svr, con->err, (ub4) OCI_DEFAULT)
        )

        /* close server handle */

        OCI_HandleFree((dvoid *) con->svr, (ub4) OCI_HTYPE_SERVER);

        con->svr = NULL;
    }

    /* update internal status */

    if (res == TRUE)
    {
        if (OCILib.version_runtime < OCI_9_0 && con->pool != NULL)
            con->pool->nb_opened--;

        con->cstate = OCI_CONN_ALLOCATED;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionLogon
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionLogon
(
    OCI_Connection *con,
    const mtext    *new_pwd,
    const mtext    *tag
)
{
    void *ostr  = NULL;
    int osize   = -1;
    boolean res = TRUE;

    OCI_CHECK(con == NULL, FALSE);

    #if OCI_VERSION_COMPILE < OCI_9_2

    OCI_NOT_USED(tag)

    #endif

    #if OCI_VERSION_COMPILE >= OCI_9_2

    if (con->pool == NULL)
    {

    #endif

    /* allocate session handle */

    if (res == TRUE)
        res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env,
                                              (dvoid **) (void *) &con->ses,
                                              (ub4) OCI_HTYPE_SESSION,
                                              (size_t) 0, (dvoid **) NULL));

    /* allocate context handle */

    if (res == TRUE)
        res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env,
                                              (dvoid **) (void *) &con->cxt,
                                              (ub4) OCI_HTYPE_SVCCTX,
                                              (size_t) 0, (dvoid **) NULL));

    /* set context server attribute */

    OCI_CALL2
    (
        res, con,

        OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                   (dvoid *) con->svr, (ub4) sizeof (con->svr),
                   (ub4) OCI_ATTR_SERVER, con->err)
    )

    /* modifiy user password if needed */

    if (new_pwd && new_pwd[0])
    {
        int   osize1 = -1;
        int   osize2 = -1;
        int   osize3 = -1;
        void *ostr1  = NULL;
        void *ostr2  = NULL;
        void *ostr3  = NULL;

        ostr1 = OCI_GetInputMetaString(con->user, &osize1);
        ostr2 = OCI_GetInputMetaString(con->pwd,  &osize2);
        ostr3 = OCI_GetInputMetaString(new_pwd,   &osize3);

        OCI_CALL2
        (
            res, con,

            OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                       (dvoid *) con->ses, (ub4) sizeof(con->ses),
                       (ub4) OCI_ATTR_SESSION, con->err)
        )

        OCI_CALL2
        (
            res, con,

            OCIPasswordChange(con->cxt, con->err,
                              (OraText *) ostr1, (ub4) osize1,
                              (OraText *) ostr2, (ub4) osize2,
                              (OraText *) ostr3, (ub4) osize3,
                              OCI_AUTH)
        )

        OCI_ReleaseMetaString(ostr1);
        OCI_ReleaseMetaString(ostr2);
        OCI_ReleaseMetaString(ostr3);


        if (res == TRUE)
        {
            OCI_FREE(con->pwd);

            con->pwd = mtsdup(new_pwd);
        }
    }
    else
    {
        /* set session login attribute */

        if ((res == TRUE) && (con->user != NULL) && (con->user[0] != 0))
        {
            osize = -1;
            ostr  = OCI_GetInputMetaString(con->user, &osize);

            OCI_CALL2
            (
                res, con,

                OCIAttrSet((dvoid *) con->ses,(ub4)  OCI_HTYPE_SESSION,
                           (dvoid *) ostr, (ub4) osize,
                           (ub4) OCI_ATTR_USERNAME, con->err)
            )

            OCI_ReleaseMetaString(ostr);
        }

        /* set session password attribute */

        if ((res == TRUE) && (con->pwd != NULL) && (con->pwd[0] != 0))
        {
            osize = -1;
            ostr  = OCI_GetInputMetaString(con->pwd, &osize);

            OCI_CALL2
            (
                res, con,

                OCIAttrSet((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION,
                           (dvoid *) ostr, (ub4) osize,
                           (ub4) OCI_ATTR_PASSWORD, con->err)
            )

            OCI_ReleaseMetaString(ostr);
        }

        /* start session */

        if (res == TRUE)
        {
            ub4 credt = OCI_CRED_RDBMS;

            if  (((con->user == NULL) || (con->user[0] == 0)) &&
                 ((con->pwd  == NULL) || (con->pwd[0]  == 0)))
            {
                credt = OCI_CRED_EXT;
            }

            OCI_CALL2
            (
                res, con,

                OCISessionBegin(con->cxt, con->err, con->ses, credt, con->mode)
            )

            /* This call has moved after OCISessionBegin() call to
               enable connection pooling (an error ORA-24324 was thrown if
               the session handle was set to the service context handle
               before OCISessionBegin()

               note  : from v3.7.0, OCISessionBegin() is not used anymore
                       for OCI managed pools
               */

            OCI_CALL2
            (
                res, con,

                OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                           (dvoid *) con->ses, (ub4) sizeof(con->ses),
                           (ub4) OCI_ATTR_SESSION, con->err)
            )
        }
    }

    #if OCI_VERSION_COMPILE >= OCI_9_2

}
else
{
    if (OCILib.version_runtime >= OCI_9_0)
    {

        ub4 mode       = OCI_DEFAULT;
        boolean found  = FALSE;
        void *ostr_tag = NULL;
        int osize_tag  = 0;

        OraText *ostr_ret = NULL;
        ub4 osize_ret     = 0;

        osize = -1;
        ostr  = OCI_GetInputMetaString(con->pool->name, &osize);

        if (con->pool->htype == OCI_HTYPE_CPOOL)
        {
            mode = OCI_SESSGET_CPOOL;
        }
        else
        {
            mode = OCI_SESSGET_SPOOL;

            if (tag != NULL)
            {
                osize_tag = -1;
                ostr_tag  = OCI_GetInputMetaString(tag, &osize_tag);
            }
        }

        OCI_CALL2
        (
            res, con,

            OCISessionGet(OCILib.env, con->err, &con->cxt, con->pool->authp,
                          (dvoid *) ostr, (ub4) osize, ostr_tag, osize_tag,
                          &ostr_ret, &osize_ret, &found, mode)
        )

        if (res == TRUE)
        {
            con->ses = (OCISession *) con->pool->authp;

            if (found == TRUE)
            {
                OCI_SetSessionTag(con, tag);
            }
        }
    }
}

    #endif

    /* check for success */

    if (res == TRUE)
    {
        /* get server version */

        OCI_GetVersionServer(con);

        if (!(con->mode & OCI_PRELIM_AUTH))
        {
            /* create default transaction object */

            con->trs = OCI_TransactionCreate(con, 1, OCI_TRANS_READWRITE, NULL);

            /* start transaction */

            res = OCI_TransactionStart(con->trs);
        }
    }

    /* set OCILIB's driver layer name attribute */

    #if OCI_VERSION_COMPILE >= OCI_11_1

    if ((res == TRUE) && (OCILib.version_runtime >= OCI_11_1) && (con->ver_num >= OCI_11_1))
    {
        osize = -1;
        ostr  = OCI_GetInputMetaString(OCILIB_DRIVER_NAME, &osize);

        OCI_CALL2
        (
            res, con,

            OCIAttrSet((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION,
                       (dvoid *) ostr, (ub4) osize,
                       (ub4) OCI_ATTR_DRIVER_NAME, con->err)
        )

        OCI_ReleaseMetaString(ostr);
    }

    #endif

    /* update internal status */

    if (res == TRUE)
    {
        if (OCILib.version_runtime < OCI_9_0 && con->pool != NULL)
            con->pool->nb_busy++;

        con->cstate = OCI_CONN_LOGGED;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionLogOff
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionLogOff
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK(con == NULL, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_LOGGED, FALSE);

    /* deassociate connection from existing subscriptions */

    OCI_SubscriptionDetachConnection(con);

    /* free all statements */

    OCI_ListForEach(con->stmts, (POCI_LIST_FOR_EACH) OCI_StatementClose);
    OCI_ListClear(con->stmts);

    /* free all transactions */

    OCI_ListForEach(con->trsns, (POCI_LIST_FOR_EACH) OCI_TransactionClose);
    OCI_ListClear(con->trsns);

    /* free all type info objects */

    OCI_ListForEach(con->tinfs, (POCI_LIST_FOR_EACH) OCI_TypeInfoClose);
    OCI_ListClear(con->tinfs);

    /* close opened files */

    if (con->nb_files > 0)
    {
        OCILobFileCloseAll(con->cxt, con->err);
    }

    /* close session */

    #if OCI_VERSION_COMPILE >= OCI_9_2

    if (con->pool == NULL)
    {

    #endif

    /* close any server files not explicitly closed - no check of return code */

    if ((con->cxt != NULL) && (con->err != NULL) && (con->ses != NULL))
    {
        OCI_CALL2
        (
            res, con,

            OCISessionEnd(con->cxt, con->err, con->ses, (ub4) OCI_DEFAULT)
        )

        /* close session handle */

        if (con->ses != NULL)
        {
            OCI_HandleFree((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION);

            con->ses = NULL;
        }

        /* close context handle */

        if (con->cxt != NULL)
        {
            OCI_HandleFree((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX);

            con->cxt = NULL;
        }
    }

    #if OCI_VERSION_COMPILE >= OCI_9_2

}
else
{
    if (OCILib.version_runtime >= OCI_9_0)
    {
        void *ostr = NULL;
        int osize  = 0;
        ub4 mode   = OCI_DEFAULT;

        if ((con->sess_tag != NULL) && (con->pool->htype == (ub4) OCI_HTYPE_SPOOL))
        {
            osize = -1;
            ostr  = OCI_GetInputMetaString(con->sess_tag, &osize);
            mode  = OCI_SESSRLS_RETAG;
        }

        /* set context transaction attribute to NULL
           If not done, OCISessionRelease() genarates a segfault if
           a transaction to set on the service context handle

           Once again, OCI bug ? Need to report that on metalink ...
        */

        OCI_CALL2
        (
            res, con,

            OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX,
                       (dvoid *) NULL, (ub4) 0,
                       (ub4) OCI_ATTR_TRANS,con->err)
        )

        OCI_CALL2
        (
            res, con,

            OCISessionRelease(con->cxt, con->err, ostr, (ub4) osize, mode)
        )

        con->cxt = NULL;
    }
}

    #endif

    /* update internal status */

    if (res == TRUE)
    {
        con->cstate = OCI_CONN_ATTACHED;

        if (OCILib.version_runtime < OCI_9_0 && con->pool != NULL)
            con->pool->nb_busy--;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ConnectionClose
(
    OCI_Connection *con
)
{
    OCI_CHECK(con == NULL, FALSE);

    /* clear server output resources */

    OCI_ServerDisableOutput(con);

    /* lofoff and detatch form server */

    OCI_ConnectionLogOff(con);
    OCI_ConnectionDetach(con);
    OCI_ConnectionDeallocate(con);

    /* free internal lists */

    OCI_ListFree(con->stmts);
    OCI_ListFree(con->trsns);
    OCI_ListFree(con->tinfs);

    if (con->pool == NULL)
    {
        /* free strings */

        OCI_FREE(con->db);
        OCI_FREE(con->user);
        OCI_FREE(con->pwd);
    }

    OCI_FREE(con->fmt_date);
    OCI_FREE(con->fmt_num);
    OCI_FREE(con->ver_str);
    OCI_FREE(con->sess_tag);

    con->stmts = NULL;
    con->trsns = NULL;
    con->tinfs = NULL;

    return TRUE;
}

/* ********************************************************************************************* *
 *                             PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_ConnectionCreate
(
    const mtext *db,
    const mtext *user,
    const mtext *pwd,
    unsigned int mode
)
{
    OCI_Connection * con;

    /* let's be sure OCI_Initialize() has been called */

    OCI_CHECK_INITIALIZED(NULL);

    con = OCI_ConnectionAllocate(NULL, db, user, pwd, mode);

    if (con != NULL)
    {
        if (OCI_ConnectionAttach(con)            == FALSE ||
            OCI_ConnectionLogon(con, NULL, NULL) == FALSE)
        {
            OCI_ConnectionFree(con);
            con = NULL;
        }
    }

    OCI_RESULT(con != NULL);

    return con;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ConnectionFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ConnectionFree
(
    OCI_Connection *con
)
{
    boolean res    = TRUE;
    OCI_Error *err = NULL;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    /* clear connection reference from current error object */

    err = OCI_ErrorGet(FALSE, FALSE);

    if (err != NULL && err->con == con)
        err->con = NULL;

    OCI_FREE(con->sess_tag);

    if (con->pool != NULL)
    {
        res = OCI_ConnectionLogOff(con);

        if (OCILib.version_runtime >= OCI_9_0)
            OCI_ConnectionDetach(con);
    }
    else
    {
        res = OCI_ConnectionClose(con);
        OCI_ListRemove(OCILib.cons, con);
        OCI_FREE(con);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Commit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Commit
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_CALL2
    (
        res, con,

        OCITransCommit(con->cxt, con->err, (ub4) OCI_DEFAULT)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Rollback
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Rollback
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_CALL2
    (
        res, con,

        OCITransRollback(con->cxt, con->err, (ub4) OCI_DEFAULT)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetAutoCommit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetAutoCommit
(
    OCI_Connection *con,
    boolean         enable
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_RESULT(TRUE);

    con->autocom = enable;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetAutoCommit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_GetAutoCommit
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_RESULT(TRUE);

    return con->autocom;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IsConnected
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IsConnected
(
    OCI_Connection *con
)
{
    boolean res = TRUE;
    ub4 status  = 0;
    ub4 size    = (ub4) sizeof(status);

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_CALL2
    (
        res, con,

        OCIAttrGet((dvoid **) con->svr, (ub4) OCI_HTYPE_SERVER,
                   (dvoid *) &status, (ub4 *) &size,
                   (ub4) OCI_ATTR_SERVER_STATUS, con->err)

    )

    OCI_RESULT(res);

    return (status == OCI_SERVER_NORMAL);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUserData
 * --------------------------------------------------------------------------------------------- */

void * OCI_API OCI_GetUserData
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return con->usrdata;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetSetData
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetUserData
(
    OCI_Connection *con,
    void           *data
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_RESULT(TRUE);

    con->usrdata = data;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetSessionTag
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetSessionTag
(
    OCI_Connection *con,
    const mtext    *tag
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_RESULT(TRUE);

    OCI_FREE(con->sess_tag);

    #if OCI_VERSION_COMPILE >= OCI_9_2

    if ((tag              != NULL                 ) &&
        (con->pool        != NULL                 ) &&
        (con->pool->htype == (ub4) OCI_HTYPE_SPOOL))
    {
        con->sess_tag = mtsdup(tag);

        res = (con->sess_tag != NULL);
    }

    #else

    OCI_NOT_USED(tag)

    #endif

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUserData
 * OCI_GetSessionTag */

const mtext * OCI_API OCI_GetSessionTag
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return con->sess_tag;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDatabase
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_GetDatabase
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return (const mtext *) con->db;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetUserName
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_GetUserName
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return (const mtext *) con->user;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetPassword
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_GetPassword
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return (const mtext *) con->pwd;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetPassword
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetPassword
(
    OCI_Connection *con,
    const mtext    *password
)
{
    boolean res = TRUE;
    ub4 mode    = OCI_DEFAULT;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, password, FALSE);

    if (con->cstate != OCI_CONN_LOGGED)
    {
        OCI_ConnectionLogon(con, password, NULL);
    }
    else
    {
        int   osize1 = -1;
        int   osize2 = -1;
        int   osize3 = -1;
        void *ostr1  = NULL;
        void *ostr2  = NULL;
        void *ostr3  = NULL;

        ostr1 = OCI_GetInputMetaString(con->user,  &osize1);
        ostr2 = OCI_GetInputMetaString(con->pwd,   &osize2);
        ostr3 = OCI_GetInputMetaString(password,   &osize3);

        OCI_CALL2
        (
            res, con,

            OCIPasswordChange(con->cxt, con->err,
                              (OraText *) ostr1, (ub4) osize1,
                              (OraText *) ostr2, (ub4) osize2,
                              (OraText *) ostr3, (ub4) osize3,
                              mode)
        )

        OCI_ReleaseMetaString(ostr1);
        OCI_ReleaseMetaString(ostr2);
        OCI_ReleaseMetaString(ostr3);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetUserPassword
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetUserPassword
(
    const mtext *db,
    const mtext *user,
    const mtext *pwd,
    const mtext *new_pwd
)
{
    OCI_Connection * con = NULL;
    boolean res          = FALSE;

    /* let's be sure OCI_Initialize() has been called */

    OCI_CHECK_INITIALIZED(FALSE);

    con = OCI_ConnectionAllocate(NULL, db, user, pwd, OCI_AUTH);

    if (con != NULL)
    {
        if (OCI_ConnectionAttach(con)               == FALSE ||
            OCI_ConnectionLogon(con, new_pwd, NULL) == FALSE)
        {
            OCI_ConnectionFree(con);
            con = NULL;
        }
    }

    if (con != NULL)
    {
        res = TRUE;
        OCI_ConnectionFree(con);
    }
    else
        res = FALSE;

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetSessionMode
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetSessionMode
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return con->mode;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetVersionServer
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_GetVersionServer
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    /* no version available in prelim mode */

    if ((con->ver_str == NULL) && (!(con->mode & OCI_PRELIM_AUTH)))
    {
        res = FALSE;

        con->ver_str = (mtext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(mtext),
                                              (size_t) (OCI_SIZE_BUFFER + 1),
                                              FALSE);

        if (con->ver_str != NULL)
        {
            int osize  = OCI_SIZE_BUFFER * (int) sizeof(mtext);
            void *ostr = NULL;
            mtext *p   = NULL;

            con->ver_str[0] = 0;

            res = TRUE;

            ostr = OCI_GetInputMetaString(con->ver_str, &osize);

            OCI_CALL2
            (
                res, con,

                OCIServerVersion((dvoid *) con->cxt, con->err,
                                 (OraText *) ostr, (ub4) osize,
                                 (ub1) OCI_HTYPE_SVCCTX)
            )

            OCI_GetOutputMetaString(ostr, con->ver_str, &osize);

            OCI_ReleaseMetaString(ostr);

            if (res == TRUE)
            {
                int ver_maj, ver_min, ver_rev;

                ver_maj = ver_min = ver_rev = 0;

                con->ver_str[osize / (int) sizeof(mtext)] = 0;

                /* parse server version string to find the version information
                 **/

                for (p = con->ver_str; (p != NULL) && (*p != 0); p++)
                {
                    if (mtisdigit(*p) &&
                        (*(p + (size_t) 1) != 0) &&
                        (*(p + (size_t) 1) == MT('.') || (*(p + (size_t) 2) == MT('.') )))
                    {
                        if (OCI_NB_ARG_VERSION == mtsscanf(p, MT("%d.%d.%d"),
                                                           (int *) &ver_maj,
                                                           (int *) &ver_min,
                                                           (int *) &ver_rev))
                        {
                            con->ver_num = ver_maj*100 + ver_min*10 + ver_rev;
                        }

                        break;
                    }
                }
            }
            else
                OCI_FREE(con->ver_str);
        }
    }

    OCI_RESULT(res);

    return con->ver_str;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerMajorVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetServerMajorVersion
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    if (con->ver_num == OCI_UNKNOWN)
        OCI_GetVersionServer(con);

    OCI_RESULT(con->ver_num != OCI_UNKNOWN);

    return (unsigned int) OCI_VER_MAJ(con->ver_num);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerMinorVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetServerMinorVersion
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    if (con->ver_num == OCI_UNKNOWN)
        OCI_GetVersionServer(con);

    OCI_RESULT(con->ver_num != OCI_UNKNOWN);

    return OCI_VER_MIN(con->ver_num);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetServerRevisionVersion
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetServerRevisionVersion
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    if (con->ver_num == OCI_UNKNOWN)
        OCI_GetVersionServer(con);

    OCI_RESULT(con->ver_num != OCI_UNKNOWN);

    return (unsigned int) OCI_VER_MAJ(con->ver_num);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetTransaction
 * --------------------------------------------------------------------------------------------- */

OCI_Transaction * OCI_API OCI_GetTransaction
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return con->trs;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetTransaction
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetTransaction
(
    OCI_Connection  *con,
    OCI_Transaction *trans
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_TRANSACTION, trans, FALSE);

    res = OCI_TransactionStop(con->trs);

    if (res == TRUE)
        con->trs = trans;

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetVersionConnection
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_GetVersionConnection
(
    OCI_Connection *con
)
{
    int v1, v2;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    v1 = OCI_GetOCIRuntimeVersion();
    v2 = OCI_GetServerMajorVersion(con);

    OCI_RESULT(TRUE);

    /* return the minimum supported version */

    return (OCILib.version_runtime > con->ver_num) ? con->ver_num : OCILib.version_runtime;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetDefaultFormatDate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetDefaultFormatDate
(
    OCI_Connection *con,
    const mtext    *format
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_FREE(con->fmt_date);

    con->fmt_date = mtsdup(format ? format : OCI_STRING_FORMAT_DATE);

    res = (con->fmt_date != NULL);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDefaultFormatDate
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_GetDefaultFormatDate
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    if (con->fmt_date == NULL)
        OCI_SetDefaultFormatDate(con, NULL);

    return (con->fmt_date);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetDefaultFormatNumeric
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetDefaultFormatNumeric
(
    OCI_Connection *con,
    const mtext    *format
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_FREE(con->fmt_num);

    con->fmt_num = mtsdup(format ? format : OCI_STRING_FORMAT_NUM);

    res = (con->fmt_num != NULL);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_GetDefaultFormatNumeric
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_GetDefaultFormatNumeric
(
    OCI_Connection *con
)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    if (con->fmt_num == NULL)
        OCI_SetDefaultFormatNumeric(con, NULL);

    return (con->fmt_num);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Break
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Break
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_CALL2
    (
        res, con,

        OCIBreak((dvoid *) con->cxt, con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ServerEnableOutput
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ServerEnableOutput
(
    OCI_Connection *con,
    unsigned int    bufsize,
    unsigned int    arrsize,
    unsigned int    lnsize
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    /* initialize the output buffer on server side */

    if (con->svopt == NULL)
    {
        con->svopt = (OCI_ServerOutput *) OCI_MemAlloc(OCI_IPC_SERVER_OUPUT,
                                                       sizeof(*con->svopt),
                                                       (size_t) 1, TRUE);

        res = (con->svopt != NULL);
    }

    /* allocation internal buffer if needed */

    if ((res == TRUE) && (con->svopt->arrbuf == NULL))
    {
        unsigned int charsize = sizeof(dtext);

        /* check params ranges ( Oracle 10g increased the size of output line */

        if (con->ver_num >= OCI_10_2)
        {
            if (lnsize < OCI_OUPUT_LSIZE)
                lnsize = OCI_OUPUT_LSIZE;
            else if (lnsize > OCI_OUPUT_LSIZE_10G)
                lnsize = OCI_OUPUT_LSIZE_10G;
        }
        else
        {
            if (lnsize > OCI_OUPUT_LSIZE)
                lnsize = OCI_OUPUT_LSIZE;
        }

        con->svopt->arrsize = arrsize;
        con->svopt->lnsize  = lnsize;

        /* allocate internal string (line) array */

        con->svopt->arrbuf =

            (ub1 *) OCI_MemAlloc(OCI_IPC_STRING,
                                 ((size_t)(con->svopt->lnsize + 1)) * charsize,
                                 (size_t) con->svopt->arrsize, TRUE);

        res = (con->svopt->arrbuf != NULL);
    }

    if (res == TRUE)
    {
        if (con->svopt->stmt == NULL)
            con->svopt->stmt = OCI_StatementCreate(con);

        if (con->svopt->stmt != NULL)
        {
            /* enable server ouput */

            res = OCI_Prepare(con->svopt->stmt,
                              MT("BEGIN DBMS_OUTPUT.ENABLE(:n); END;"));

            res = res && OCI_BindUnsignedInt(con->svopt->stmt, MT(":n"), &bufsize);

            if (bufsize == 0)
                res = OCI_BindSetNull(OCI_GetBind(con->svopt->stmt, 1));

            res = res && OCI_Execute(con->svopt->stmt);

            /* prepare the retreival statement call */

            con->svopt->cursize = con->svopt->arrsize;

            res = res && OCI_Prepare(con->svopt->stmt,
                                     MT("BEGIN DBMS_OUTPUT.GET_LINES(:s, :i); END;"));

            res = res && OCI_BindArrayOfStrings(con->svopt->stmt,
                                                MT(":s"),
                                                (dtext *) con->svopt->arrbuf,
                                                con->svopt->lnsize,
                                                con->svopt->arrsize);

            res = res && OCI_BindUnsignedInt(con->svopt->stmt,
                                             MT(":i"),
                                             &con->svopt->cursize);
        }
    }

    if (res == FALSE)
        OCI_ServerDisableOutput(con);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ServerDisableOutput
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ServerDisableOutput
(
    OCI_Connection *con
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    if (con->svopt != NULL)
    {
        res = res && OCI_ExecuteStmt(con->svopt->stmt,
                                     MT("BEGIN DBMS_OUTPUT.DISABLE(); END;"));

        res = res && OCI_StatementFree(con->svopt->stmt);

        OCI_FREE(con->svopt->arrbuf);
        OCI_FREE(con->svopt);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ServerGetOutput
 * --------------------------------------------------------------------------------------------- */

const dtext * OCI_API OCI_ServerGetOutput
(
    OCI_Connection *con
)
{
    boolean res = TRUE;
    dtext *str  = NULL;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK(con->svopt == NULL, FALSE);

    if (con->svopt->curpos == 0 || con->svopt->curpos >= con->svopt->cursize)
    {
        con->svopt->cursize = con->svopt->arrsize;
        res                 = OCI_Execute(con->svopt->stmt);
        con->svopt->curpos  = 0;
    }

    if ((res == TRUE) & (con->svopt->cursize > 0))
    {
        unsigned int charsize = sizeof(dtext);

        str = (dtext*) (con->svopt->arrbuf +
                        (size_t) (((con->svopt->lnsize + 1) * charsize) * con->svopt->curpos++));
    }

    OCI_RESULT(res);

    return (const dtext *) str;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_SetTrace
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_SetTrace
(
    OCI_Connection *con,
    unsigned int    trace,
    const mtext    *value
)
{
    boolean res = TRUE;
    mtext *str  = NULL;

    #if OCI_VERSION_COMPILE >= OCI_10_1
    ub4 attrib = 0;
    #endif

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    /* allocate trace info structure only if trace functions are used */

    if (con->trace == NULL)
    {
        con->trace = (OCI_TraceInfo *) OCI_MemAlloc(OCI_IPC_TRACE_INFO,
                                                    sizeof(*con->trace),
                                                    (size_t) 1, TRUE);
        res = (con->trace != NULL);
    }

    /* set trace properties */

    if (con->trace != NULL)
    {
        switch (trace)
        {
            case OCI_TRC_IDENTITY:

                #if OCI_VERSION_COMPILE >= OCI_10_1

                attrib = OCI_ATTR_CLIENT_IDENTIFIER;

                #endif
                con->trace->identifier[0] = 0;

                mtsncat(con->trace->identifier, value, OCI_SIZE_TRACE_ID);

                str = con->trace->identifier;

                break;

            case OCI_TRC_MODULE:

                #if OCI_VERSION_COMPILE >= OCI_10_1

                attrib = OCI_ATTR_MODULE;

                #endif
                con->trace->module[0] = 0;

                mtsncat(con->trace->module, value, OCI_SIZE_TRACE_MODULE);

                str = con->trace->module;

                break;

            case OCI_TRC_ACTION:

                #if OCI_VERSION_COMPILE >= OCI_10_1

                attrib = OCI_ATTR_ACTION;

                #endif
                con->trace->action[0] = 0;

                mtsncat(con->trace->action, value, OCI_SIZE_TRACE_ACTION);

                str = con->trace->action;

                break;

            case OCI_TRC_DETAIL:

                #if OCI_VERSION_COMPILE >= OCI_10_1

                attrib = OCI_ATTR_CLIENT_INFO;

                #endif
                con->trace->info[0] = 0;

                mtsncat(con->trace->info, value,  OCI_SIZE_TRACE_INF0);

                str = con->trace->info;

                break;

            default:

                res = FALSE;
        }
    }

    #if OCI_VERSION_COMPILE >= OCI_10_1

    /* On success, we give the value to Oracle to record it in system session
     *view */

    if (res == TRUE)
    {
        void *ostr = NULL;
        int osize  = -1;

        ostr = OCI_GetInputMetaString(str, &osize);

        if (str == NULL)
            osize = 0;

        OCI_CALL2
        (
            res, con,

            OCIAttrSet((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION,
                       (dvoid *) ostr, (ub4) osize, attrib, con->err)
        )

        OCI_ReleaseMetaString(ostr);
    }

    #endif

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TraceGet
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_GetTrace
(
    OCI_Connection *con,
    unsigned int    trace
)
{
    const mtext *str = NULL;
    boolean res      = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    if (con->trace != NULL)
    {
        switch (trace)
        {
            case OCI_TRC_IDENTITY:

                str = con->trace->identifier;
                break;

            case OCI_TRC_MODULE:

                str = con->trace->module;
                break;

            case OCI_TRC_ACTION:

                str = con->trace->action;
                break;

            case OCI_TRC_DETAIL:

                str = con->trace->info;
                break;

            default:

                res = FALSE;
        }
    }

    OCI_RESULT(res);

    return str;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_Ping
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_Ping
(
    OCI_Connection *con
)
{
    boolean res = TRUE;
    boolean ret = FALSE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    #if OCI_VERSION_COMPILE >= OCI_10_2

    if (OCILib.version_runtime >= OCI_10_2)
    {
        OCI_CALL2
        (
            res, con,

            OCIPing(con->cxt, con->err, (ub4) OCI_DEFAULT)

        )

        ret = res;
    }

    #endif

    OCI_RESULT(res);

    return ret;
}

