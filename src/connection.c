/*
   +----------------------------------------------------------------------+   
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2009 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+ 
*/

/* ------------------------------------------------------------------------ *
 * $Id: connection.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"


/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionAllocate
 * ------------------------------------------------------------------------ */

OCI_Connection * OCI_ConnectionAllocate(OCI_ConnPool *pool, const mtext *db, 
                                        const mtext *user,  const mtext *pwd, 
                                        unsigned int mode)
{
    OCI_Connection *con = NULL;
    OCI_List *list      = NULL;
    OCI_Item *item      = NULL;
    boolean res         = TRUE;

    /* create connection object */

    if (pool != NULL)
        list  = pool->cons;
    else
        list  = OCILib.cons;

    item = OCI_ListAppend(list, sizeof(*con));
 
    if (item != NULL)
    {
        con = (OCI_Connection *) item->data;

        /* create internal lists */

        con->stmts = OCI_ListCreate(OCI_IPC_STATEMENT);    
        
        if (res == TRUE)
        {
            con->tinfs = OCI_ListCreate(OCI_IPC_TYPE_INFO);    
            res = (con->tinfs != NULL);
        }

        if (res == TRUE)
        {
            con->trsns = OCI_ListCreate(OCI_IPC_TRANSACTION);
            res = (con->trsns != NULL);
        }
 
        /* set attributes */

        if (res == TRUE)
        {
            con->mode = mode;
            con->pool = pool;

            if (con->pool != NULL)
            {
                con->db     = (mtext *) db;
                con->user   = (mtext *) user;
                con->pwd    = (mtext *) pwd;
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

        /* allocate server handle */
        
        if (res == TRUE)
            res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env, 
                                                  (dvoid **) (void *) &con->svr,
                                                  (ub4) OCI_HTYPE_SERVER,
                                                  (size_t) 0, (dvoid **) NULL));

        /* allocate context handle */
        
        if (res == TRUE)
            res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env, 
                                                  (dvoid **) (void *) &con->cxt,
                                                  (ub4) OCI_HTYPE_SVCCTX, 
                                                  (size_t) 0, (dvoid **) NULL));

        /* allocate session handle */
        
        if (res == TRUE)
            res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env,
                                                  (dvoid **) (void *) &con->ses,
                                                  (ub4) OCI_HTYPE_SESSION,
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

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionDeallocate
 * ------------------------------------------------------------------------ */

boolean OCI_ConnectionDeallocate(OCI_Connection *con)
{
    OCI_CHECK(con == NULL, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE);

    /* close context handle */
    
    if (con->cxt != NULL)
       OCI_HandleFree((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX);

    /* close session handle */
    
    if (con->ses != NULL)
        OCI_HandleFree((dvoid *) con->ses, (ub4) OCI_HTYPE_SESSION);

    /* close server handle */
    if (con->svr != NULL)
        OCI_HandleFree((dvoid *) con->svr, (ub4) OCI_HTYPE_SERVER);

     /* close error handle */
    
    if (con->err != NULL)
        OCI_HandleFree((dvoid *) con->err, (ub4) OCI_HTYPE_ERROR);

    con->cxt = NULL;
    con->ses = NULL;
    con->svr = NULL;
    con->err = NULL;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionAttach
 * ------------------------------------------------------------------------ */

boolean OCI_ConnectionAttach(OCI_Connection *con)
{
    void *ostr  = NULL;
    int osize   = -1;
    boolean res = TRUE;
    ub4 cmode   = OCI_DEFAULT;

    OCI_CHECK(con == NULL, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_ALLOCATED, FALSE);

    /* attach server handle to service name */

#if OCI_VERSION_COMPILE >= OCI_9

    if (OCILib.ver_runtime >= OCI_9 && con->pool != NULL)
    {
        ostr  = OCI_GetInputMetaString(con->pool->name, &osize);
        cmode = OCI_CPOOL;
    }
    else

#endif

    {
        ostr  = OCI_GetInputMetaString(con->db, &osize);
    }

    OCI_CALL2
    (
        res, con, 
        
        OCIServerAttach(con->svr, con->err,(OraText *) ostr, (sb4) osize, cmode)
    )
    
    OCI_ReleaseMetaString(ostr);

    /* handle errors */
    
    if (res == TRUE)
    {
        if (OCILib.ver_runtime < OCI_9 && con->pool != NULL)
            con->pool->nb_opened++;

        con->cstate = OCI_CONN_ATTACHED;
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionDetach
 * ------------------------------------------------------------------------ */

boolean OCI_ConnectionDetach(OCI_Connection *con)
{
    boolean res = TRUE;

    OCI_CHECK(con == NULL, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_ATTACHED, FALSE);

    /* detach from the oracle server */
    
    OCI_CALL2
    (
        res, con, 
        
        OCIServerDetach(con->svr, con->err, (ub4) OCI_DEFAULT)
    )
 
   /* update internal status */

    if (res == TRUE)
    {
        if (OCILib.ver_runtime < OCI_9 && con->pool != NULL)
            con->pool->nb_opened--;

        con->cstate = OCI_CONN_ALLOCATED;
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionLogon
 * ------------------------------------------------------------------------ */

boolean OCI_ConnectionLogon(OCI_Connection *con)
{
    void *ostr  = NULL;
    int osize   = -1;
    boolean res = TRUE;
    
    OCI_CHECK(con == NULL, FALSE);

    /* set context server attribute */
    
    OCI_CALL2
    (
        res, con, 
        
        OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX, 
                   (dvoid *) con->svr, (ub4) sizeof (con->svr), 
                   (ub4) OCI_ATTR_SERVER, con->err)
    )

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
 
       /* This call has moved after OCISessionBegin() call to enable connection
          pooling (an error ORA-24324 was thrown is the session handle was set to
          the service context handle before OCISessionBegin() */
    
        OCI_CALL2
        (
            res, con,
            
            OCIAttrSet((dvoid *) con->cxt, (ub4) OCI_HTYPE_SVCCTX, 
                       (dvoid *) con->ses, (ub4) sizeof(con->ses), 
                       (ub4) OCI_ATTR_SESSION, con->err))

    }
    
    /* check for success */

    if (res == TRUE)
    {
        /* get server version */

        OCI_GetVersionServer(con);

        /* create default transaction object */

        con->trs  = OCI_TransactionCreate(con, 1, OCI_TRANS_READWRITE, NULL);

        /* start transaction */
        
        res = OCI_TransactionStart(con->trs);
    }


    /* set OCILIB's driver layer name attribute */

#if OCI_VERSION_COMPILE >= OCI_11

    if ((res == TRUE) && (OCILib.ver_runtime >= OCI_11) && (con->ver_maj >= OCI_11))
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
        if (OCILib.ver_runtime < OCI_9 && con->pool != NULL)
            con->pool->nb_busy++;

        con->cstate = OCI_CONN_LOGGED;
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionLogOff
 * ------------------------------------------------------------------------ */

boolean OCI_ConnectionLogOff(OCI_Connection *con)
{
    boolean res = TRUE;

    OCI_CHECK(con == NULL, FALSE);
    OCI_CHECK(con->cstate != OCI_CONN_LOGGED, FALSE);

    /* free all statements */
    
    OCI_ListForEach(con->stmts, (boolean (*)(void *)) OCI_StatementClose);
    OCI_ListClear(con->stmts);

    /* cleanup the cache */

    OCI_CALL2
    (
        res, con, 

        OCICacheFree(OCILib.env, con->err, con->cxt)
    )

    /* free all transactions */
    
    OCI_ListForEach(con->trsns, (boolean (*)(void *)) OCI_TransactionClose);
    OCI_ListClear(con->trsns);

    /* free all type info objects */
    
    OCI_ListForEach(con->tinfs, (boolean (*)(void *)) OCI_TypeInfoClose);
    OCI_ListClear(con->tinfs);

   /* close any server files not explicitly closed - no check of return code */
   
    if (con->nb_files > 0)
    {
        OCILobFileCloseAll(con->cxt, con->err);
    }

    /* close session */
   
    if ((con->cxt != NULL) && (con->err != NULL) && (con->ses != NULL))
    {
         OCI_CALL2
         (
            res, con, 
            
            OCISessionEnd(con->cxt, con->err, con->ses, (ub4) OCI_DEFAULT)
         )
    }

    /* update internal status */

    if (res == TRUE)
    {
        con->cstate = OCI_CONN_ATTACHED;

        if (OCILib.ver_runtime < OCI_9 && con->pool != NULL)
            con->pool->nb_busy--;
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionClose
 * ------------------------------------------------------------------------ */

boolean OCI_ConnectionClose(OCI_Connection *con)
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
    OCI_FREE(con->version);

    con->stmts = NULL;
    con->trsns = NULL;
    con->tinfs = NULL;

    return TRUE;
}

/* ************************************************************************ *
 *                             PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionCreate
 * ------------------------------------------------------------------------ */

OCI_Connection * OCI_API OCI_ConnectionCreate(const mtext *db, 
                                              const mtext *user,
                                              const mtext *pwd,
                                              unsigned int mode)
{
    OCI_Connection * con;
    
    /* let's be sure OCI_Initialize() has been called */

    OCI_CHECK_INITIALIZED(NULL);

    con = OCI_ConnectionAllocate(NULL, db, user, pwd, mode);

    if (con != NULL)
    {
        if (OCI_ConnectionAttach(con) == FALSE ||
            OCI_ConnectionLogon(con)  == FALSE)
        {
            OCI_ConnectionFree(con);
            con = NULL;
        }
    }

    OCI_RESULT(con != NULL);

    return con;
}

/* ------------------------------------------------------------------------ *
 * OCI_ConnectionFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ConnectionFree(OCI_Connection *con)
{
    boolean res    = TRUE;
    OCI_Error *err = NULL;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    /* clear connection reference from current error object */
    
    err = OCI_ErrorGet(FALSE);

    if (err != NULL && err->con == con)
        err->con = NULL;
        
    if (con->pool != NULL)
    {
        res = OCI_ConnectionLogOff(con);
    
        if (OCILib.ver_runtime >= OCI_9)
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

/* ------------------------------------------------------------------------ *
 * OCI_Commit
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_Commit(OCI_Connection *con)
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

/* ------------------------------------------------------------------------ *
 * OCI_Rollback
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_Rollback(OCI_Connection *con)
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

/* ------------------------------------------------------------------------ *
 * OCI_SetAutoCommit
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetAutoCommit(OCI_Connection *con, boolean enable)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_RESULT(TRUE);

    con->autocom = enable;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetAutoCommit
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_GetAutoCommit(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_RESULT(TRUE);

    return con->autocom;
}

/* ------------------------------------------------------------------------ *
 * OCI_IsConnected
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_IsConnected(OCI_Connection *con)
{
    boolean res = TRUE;
    ub4 status  = 0;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_CALL2
    (
        res, con, 
        
        OCIAttrGet((dvoid **) con->svr, (ub4) OCI_HTYPE_SERVER,
                   (dvoid *) &status, (ub4 *) NULL, 
                   (ub4) OCI_ATTR_SERVER_STATUS, con->err)

    )

    OCI_RESULT(res);

    return (status == OCI_SERVER_NORMAL);
}

/* ------------------------------------------------------------------------ *
 * OCI_GetUserData
 * ------------------------------------------------------------------------ */

void * OCI_API OCI_GetUserData(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_RESULT(TRUE);

    return con->usrdata;  
}

/* ------------------------------------------------------------------------ *
 * OCI_SetSetData
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetUserData(OCI_Connection *con, void * data)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_RESULT(TRUE);

    con->usrdata = data;  

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetDatabase
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetDatabase(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return (const mtext *) con->db;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetUserName
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetUserName(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return (const mtext *) con->user;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetPassword
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetPassword(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return (const mtext *) con->pwd;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetPassword
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetPassword(OCI_Connection *con, const mtext *password)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, password, FALSE);

    OCI_CALL2
    (
        res, con, 
        
        OCIPasswordChange(con->cxt, con->err, 
                          (OraText *) con->user, (ub4) mtextsize(con->user),
                          (OraText *) con->pwd,  (ub4) mtextsize(con->pwd),
                          (OraText *) password,  (ub4) mtextsize(password),
                          (ub4) OCI_DEFAULT)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetSessionMode
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetSessionMode(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return con->mode;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetVersionServer
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetVersionServer(OCI_Connection *con)
{
    boolean res = FALSE;
 
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    if (con->version == NULL)
    {
        con->version = (mtext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(mtext), 
                                              OCI_SIZE_BUFFER + 1, FALSE);

        if (con->version != NULL)
        {
            int osize  = OCI_SIZE_BUFFER * sizeof(mtext);
            void *ostr = NULL;
            mtext *p   = NULL;

            con->version[0] = 0;
            
            res  = TRUE;
 
            ostr = OCI_GetInputMetaString(con->version, &osize);
  
            OCI_CALL2
            (
                res, con, 
                
                OCIServerVersion((dvoid *) con->cxt, con->err,
                                 (OraText *) ostr, (ub4) osize,
                                 (ub1) OCI_HTYPE_SVCCTX)
            )
            
            OCI_GetOutputMetaString(ostr, con->version, &osize);
                       
            OCI_ReleaseMetaString(ostr);

            if (res == TRUE)
            {
                con->version[osize / sizeof(mtext)] = 0;

                /* parse server version string to find the version information */

                for (p = con->version; (p != NULL) && (*p != 0); p++)
                {
                    if (mtisdigit(*p) &&
                        (*(p+1) != 0) &&
                        (*(p+1) == MT('.') || (*(p+2) == MT('.') )))
                    {
                        if (OCI_NB_ARG_VERSION != mtsscanf(p, MT("%d.%d.%d"),
                                                           &con->ver_maj,
                                                           &con->ver_min,
                                                           &con->ver_rev))
                        {
                            /* extracting version info failed ! */

                            con->ver_maj = 0;
                            con->ver_min = 0;
                            con->ver_rev = 0;
                        }

                        break;
                    }
                }
            }
            else
                OCI_FREE(con->version);
        }
    }

    OCI_RESULT(res);

    return con->version;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetServerMajorVersion
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetServerMajorVersion(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    if (con->ver_maj == 0)
        OCI_GetVersionServer(con);

    OCI_RESULT(con->ver_maj != OCI_UNKNOWN);

    return con->ver_maj;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetServerMinorVersion
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetServerMinorVersion(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    if (con->ver_min == 0)
        OCI_GetVersionServer(con);

    OCI_RESULT(con->ver_min != OCI_UNKNOWN);

    return con->ver_min;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetServerRevisionVersion
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetServerRevisionVersion(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    if (con->ver_rev == 0)
        OCI_GetVersionServer(con);

    OCI_RESULT(con->ver_rev != OCI_UNKNOWN);

    return con->ver_rev;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetTransaction
 * ------------------------------------------------------------------------ */

OCI_Transaction * OCI_API OCI_GetTransaction(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    return con->trs;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetTransaction
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetTransaction(OCI_Connection *con, OCI_Transaction *trans)
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

/* ------------------------------------------------------------------------ *
 * OCI_GetVersionConnection
 * ------------------------------------------------------------------------ */

 unsigned int OCI_API OCI_GetVersionConnection(OCI_Connection *con)
{
    int v1, v2;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, OCI_UNKNOWN);

    v1 = OCI_GetOCIRuntimeVersion();
    v2 = OCI_GetServerMajorVersion(con);

    OCI_RESULT(TRUE);

    /* return the minimum supported version */

    return (v1 > v2) ? v2 : v1;
}


/* ------------------------------------------------------------------------ *
 * OCI_SetDefaultFormatDate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetDefaultFormatDate(OCI_Connection *con, const mtext *format)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_FREE(con->fmt_date);

    con->fmt_date = mtsdup(format ? format : OCI_STRING_FORMAT_DATE);

    res = (con->fmt_date != NULL);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetDefaultFormatDate
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetDefaultFormatDate(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    if (con->fmt_date == NULL)
        OCI_SetDefaultFormatDate(con, NULL);

    return (con->fmt_date);
}

/* ------------------------------------------------------------------------ *
 * OCI_SetDefaultFormatNumeric
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetDefaultFormatNumeric(OCI_Connection *con, const mtext *format)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    OCI_FREE(con->fmt_num);

    con->fmt_num = mtsdup(format ? format : OCI_STRING_FORMAT_NUM);

    res = (con->fmt_num != NULL);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetDefaultFormatNumeric
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetDefaultFormatNumeric(OCI_Connection *con)
{
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    OCI_RESULT(TRUE);

    if (con->fmt_num == NULL) 
        OCI_SetDefaultFormatNumeric(con, NULL);

    return (con->fmt_num);
}

/* ------------------------------------------------------------------------ *
 * OCI_Break
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_Break(OCI_Connection *con)
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

/* ------------------------------------------------------------------------ *
 * OCI_ServerEnableOutput
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ServerEnableOutput(OCI_Connection *con, 
                                       unsigned int bufsize,
                                       unsigned int arrsize,
                                       unsigned int lnsize)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    /* initialize the output buffer on server side */

    if (con->svopt == NULL)
    {
        con->svopt  = (OCI_ServerOutput *) OCI_MemAlloc(OCI_IPC_SERVER_OUPUT,
                                                        sizeof(*con->svopt), 
                                                        1, TRUE);

        res = (con->svopt != NULL);
    }

    /* allocation internal buffer if needed */

    if ((res == TRUE) && (con->svopt->arrbuf == NULL))
    {
        /* check params ranges ( Oracle 10g increased the size of output line */

        if (con->ver_maj > OCI_10 || (con->ver_maj == OCI_10 && con->ver_min >= 2))
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

        con->svopt->arrbuf = (ub1 *) OCI_MemAlloc(OCI_IPC_STRING,
                                                  (con->svopt->lnsize + 1) * sizeof(dtext),
                                                  con->svopt->arrsize, TRUE
                                                  );

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
                res = OCI_SetNull(con->svopt->stmt, 1);

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

/* ------------------------------------------------------------------------ *
 * OCI_ServerDisableOutput
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ServerDisableOutput(OCI_Connection *con)
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

/* ------------------------------------------------------------------------ *
 * OCI_ServerGetOutput
 * ------------------------------------------------------------------------ */

const dtext * OCI_API OCI_ServerGetOutput(OCI_Connection *con)
{
    boolean res = TRUE;
    dtext *str  = NULL;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK(con->svopt == NULL, FALSE);

    if (con->svopt->curpos == 0 || con->svopt->curpos >= con->svopt->cursize)
        res = OCI_Execute(con->svopt->stmt);

    if (con->svopt->cursize > 0)
        str = (dtext*) (con->svopt->arrbuf + 
                        (((con->svopt->lnsize + 1) * sizeof(dtext)) * con->svopt->curpos++));

    OCI_RESULT(res);

    return (const dtext *) str;
}



/* ------------------------------------------------------------------------ *
 * OCI_SetTrace
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetTrace(OCI_Connection *con, unsigned int trace, 
                             const mtext *value)
{
    boolean res = TRUE;
    mtext *str  = NULL;

#if OCI_VERSION_COMPILE >= OCI_10
    ub4 attrib  = 0;
#endif

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);

    /* allocate trace info structure only if trace functions are used */

    if (con->trace == NULL)
    {
        con->trace = (OCI_TraceInfo *) OCI_MemAlloc(OCI_IPC_TRACE_INFO, 
                                                    sizeof(*con->trace), 
                                                    1, TRUE);
        res = (con->trace != NULL);
    }

    /* set trace properties */

    if (con->trace != NULL)
    {
        switch (trace)
        {
            case OCI_TRC_IDENTITY:

#if OCI_VERSION_COMPILE >= OCI_10

                attrib = OCI_ATTR_CLIENT_IDENTIFIER;

#endif
                con->trace->identifier[0] = 0;

                mtsncat(con->trace->identifier, value,
                        msizeof(con->trace->identifier));

                str = con->trace->identifier;
                
                break;
 
            case OCI_TRC_MODULE:

 #if OCI_VERSION_COMPILE >= OCI_10

                attrib = OCI_ATTR_MODULE;

#endif
                con->trace->module[0] = 0;

                mtsncat(con->trace->module, value, msizeof(con->trace->module));

                str = con->trace->module;
                
                break;

            case OCI_TRC_ACTION:

#if OCI_VERSION_COMPILE >= OCI_10

                attrib = OCI_ATTR_ACTION;

#endif
                con->trace->action[0] = 0;

                mtsncat(con->trace->action, value, msizeof(con->trace->action));

                str = con->trace->action;
                
                break;

            case OCI_TRC_DETAIL:

#if OCI_VERSION_COMPILE >= OCI_10

                attrib = OCI_ATTR_CLIENT_INFO;

#endif
                con->trace->info[0] = 0;

                mtsncat(con->trace->info, value,  msizeof(con->trace->info));

                str = con->trace->info;
                
                break;

            default:

                res = FALSE;
        }
    }

#if OCI_VERSION_COMPILE >= OCI_10

    /* On success, we give the value to Oracle to record it in system session view */

    if (res == TRUE)
    {
        void *ostr  = NULL;
        int osize   = -1;

        ostr  = OCI_GetInputMetaString(str, &osize);

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

/* ------------------------------------------------------------------------ *
 * OCI_TraceGet
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetTrace(OCI_Connection *con, unsigned int trace)
{
    const mtext *str = NULL;
    boolean res = TRUE;

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
