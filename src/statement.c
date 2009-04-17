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
 * $Id: statement.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_BindFreeAll
 * ------------------------------------------------------------------------ */

boolean OCI_BindFreeAll(OCI_Statement *stmt)
{
    int i;

    OCI_CHECK(stmt == NULL, FALSE);

    /* free user binds */

    if (stmt->ubinds != NULL)
    {
        for(i = 0; i < stmt->nb_ubinds; i++)
        {
            OCI_BindFree(stmt->ubinds[i]);
        }

        OCI_FREE(stmt->ubinds);
    }

    /* free register binds */

    if (stmt->rbinds != NULL)
    {
        for(i = 0; i < stmt->nb_rbinds; i++)
        {
            OCI_BindFree(stmt->rbinds[i]);
        }

        OCI_FREE(stmt->rbinds);
    }

    stmt->nb_ubinds = 0;
    stmt->nb_rbinds = 0;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindCheck
 * ------------------------------------------------------------------------ */

boolean OCI_BindCheck(OCI_Statement *stmt)
{
    boolean res   = TRUE;
    OCI_Bind *bnd = NULL;
    sb2 *ind      = NULL;
    ub4 i, j;

    OCI_CHECK(stmt == NULL, FALSE)
    OCI_CHECK(stmt->ubinds == NULL, TRUE);

    for(i = 0; i < stmt->nb_ubinds; i++)
    {
        bnd = stmt->ubinds[i];
        ind = (sb2 *) bnd->buf.inds;

        if (bnd->alloc == TRUE)
        {
            if (bnd->stmt->bind_array == FALSE)
            {
                /* - For big integer (64 bits), we use an OCINumber.

                   - Oracle date/time type is the only non scalar type
                     implemented by oracle through a public structure instead
                     of using a handle. So we need to copy the value
                */

                if (bnd->type == OCI_CDT_NUMERIC)
                {
                    res = OCI_NumberSet(stmt->con,
                                        (OCINumber *) bnd->buf.data,
                                        (void *) bnd->input,
                                        (uword) sizeof(big_int),
                                        bnd->subtype);
                }
                else if (bnd->type == OCI_CDT_DATETIME)
                {
                    memcpy((void *) bnd->buf.data,
                           ((OCI_Date *) bnd->input)->handle,
                           sizeof(OCIDate));
                }

#ifdef OCI_CHECK_DATASTRINGS

                else if (bnd->type == OCI_CDT_TEXT)
                {
                    /* need conversion if bind buffer was allocated */

                    int osize = -1;

                    OCI_GetOutputString(bnd->input, bnd->buf.data, &osize,
                                        sizeof(dtext), sizeof(odtext));

                }

#endif

                else
                {
                    bnd->buf.data[0] = ((OCI_Datatype *) bnd->input)->handle;

                    /* for handles, check anyway the value for null data */

                    if (ind != NULL && *ind != -1)
                        *ind = OCI_IND(bnd->buf.data);
                }

                if (res == FALSE)
                    break;
            }
            else
            {
                for (j = 0; j < bnd->buf.count; j++, ind++)
                {

                    /* - For big integer (64 bits), we use an OCINumber.

                       - Oracle date/time type is the only non scalar type
                         implemented by oracle through a public structure instead
                         of using a handle. So we need to copy the value
                    */

                    if (bnd->type == OCI_CDT_NUMERIC)
                    {

                        res = OCI_NumberSet(stmt->con,
                                           (OCINumber *) ((ub1 *) bnd->buf.data + (j*bnd->size)),
                                           (void *) (((ub1 *) bnd->input) + (j*sizeof(big_int))),
                                           (uword) sizeof(big_int), bnd->subtype);
                    }
                    else  if (bnd->type == OCI_CDT_DATETIME)
                    {
                        memcpy(((ub1 *) bnd->buf.data) + (j*bnd->size),
                               ((OCI_Date *) bnd->input[j])->handle,
                               sizeof(OCIDate));
                    }

#ifdef OCI_CHECK_DATASTRINGS

                    else if (bnd->type == OCI_CDT_TEXT)
                    {
                        /* need conversion if bind buffer was allocated */

                        int osize   = -1;
                        int offset1 = (bnd->size/sizeof(odtext))*sizeof(dtext);
                        int offset2 = bnd->size;

                        OCI_GetOutputString(((ub1 *) bnd->input)    + (j*offset1),
                                            ((ub1 *) bnd->buf.data) + (j*offset2),
                                            &osize, sizeof(dtext), sizeof(odtext));

                    }

#endif

                    else
                    {
                        bnd->buf.data[j] = ((OCI_Datatype *) bnd->input[j])->handle;

                        /* for handles, check anyway the value for null data */

                        if (ind != NULL && *ind != -1)
                            *ind = OCI_IND(bnd->buf.data[j]);
                    }


                    if (res == FALSE)
                        break;
                }
            }
        }
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindReset
 * ------------------------------------------------------------------------ */

boolean OCI_BindReset(OCI_Statement *stmt)
{
    ub4 i, j;

    OCI_CHECK(stmt == NULL, FALSE)
    OCI_CHECK(stmt->ubinds == NULL, FALSE);

    /* avoid unused param warning from compiler */

    i = j = 0;

    for(i = 0; i < stmt->nb_ubinds; i++)
    {
        OCI_Bind *bnd = stmt->ubinds[i];

        memset(bnd->buf.inds, 0, bnd->buf.count * sizeof(sb2));

#ifdef OCI_CHECK_DATASTRINGS

        if (stmt->ubinds[i]->type == OCI_CDT_TEXT)
        {
            for (j = 0; j < bnd->buf.count; j++)
            {
                /* need conversion if bind buffer was allocated */

                int osize   = -1;
                int offset1 = (bnd->size/sizeof(odtext))*sizeof(dtext);
                int offset2 = bnd->size;

                OCI_GetOutputString(((ub1 *) bnd->buf.data) + (j*offset2),
                                    ((ub1 *) bnd->input)    + (j*offset1),
                                    &osize, sizeof(odtext), sizeof(dtext));
            }
        }

#endif

    }

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindData
 * ------------------------------------------------------------------------ */

boolean OCI_BindData(OCI_Statement *stmt, void *data, ub4 size,
                     const mtext *name, ub1 type, unsigned int code,
                     unsigned int mode, unsigned int subtype,
                     OCI_TypeInfo *typinf, unsigned int nbelem)
{
    boolean res      = TRUE;
    OCI_Bind *bnd    = NULL;
    ub4 exec_mode    = OCI_DEFAULT;
    boolean is_pltbl = FALSE;
    boolean reused   = FALSE;
    ub4 *pnbelem     = NULL;
    int index        = 0;
    ub4 i;

    /* check index if necessary */

    if (res == TRUE)
    {
        if (stmt->bind_mode == OCI_BIND_BY_POS)
        {
            index = (int) mtstol(&name[1], NULL, 10);

            if (index <= 0 || index > OCI_BIND_MAX)
            {
                OCI_ExceptionOutOfBounds(stmt->con, index);
                res = FALSE;
            }
        }
    }

    /* check if the bind name has already been used */

    if (res == TRUE)
    {
        if (mode == OCI_BIND_INPUT)
        {
            int test_index = OCI_BindGetIndex(stmt, name);

            if (test_index > 0) 
            {
                if (stmt->bind_reuse == FALSE)
                {
                    OCI_ExceptionBindAlreadyUsed(stmt, name);
                    res = FALSE;
                }
                else
                {
                    bnd = stmt->ubinds[test_index-1];
                    reused = TRUE;
                }

                index = test_index;
            }
        }
    }

    /* check if we can handle another bind */

    if (res == TRUE)
    {
        if (mode == OCI_BIND_INPUT)
        {
            if (stmt->nb_ubinds >= OCI_BIND_MAX)
            {
                OCI_ExceptionMaxBind(stmt);
                res = FALSE;
            }

            /* allocate user bind array if necessary */

            if (stmt->ubinds == NULL)
            {
                stmt->ubinds = (OCI_Bind **) OCI_MemAlloc(OCI_IPC_BIND_ARRAY,
                                                          sizeof(*stmt->ubinds),
                                                          OCI_BIND_MAX, TRUE);
            }

            res = (stmt->ubinds != NULL);
        }
        else
        {
            if (stmt->nb_rbinds >= OCI_BIND_MAX)
            {
                OCI_ExceptionMaxBind(stmt);
                res = FALSE;
            }

            /* allocate register bind array if necessary */

            if (stmt->rbinds == NULL)
            {
                stmt->rbinds = (OCI_Bind **) OCI_MemAlloc(OCI_IPC_BIND_ARRAY,
                                                          sizeof(*stmt->rbinds),
                                                          OCI_BIND_MAX, TRUE);
            }

            res = (stmt->rbinds != NULL);
        }
    }

    /* checks done */

    if (res == TRUE)
    {
        /* check out the number of elements that the bind variable will hold */

        if (nbelem > 0)
        {
            /* is it a pl/sql table bind ? */

            if (stmt->type == OCI_CST_BEGIN || stmt->type == OCI_CST_DECLARE)
                is_pltbl = TRUE;
        }
        else
            nbelem = stmt->nb_iters;
    }

    /* create hash table for mapping bind names / index */

    if (res == TRUE)
    {
        if (stmt->map == NULL)
        {
            stmt->map = OCI_HashCreate(OCI_HASH_DEFAULT_SIZE, OCI_HASH_INTEGER);

            res = (stmt->map != NULL);

        }
    }

    /* allocate bind object */

    if (res == TRUE)
    {
        if (bnd == NULL)
        {
            bnd = (OCI_Bind *) OCI_MemAlloc(OCI_IPC_BIND, sizeof(*bnd),  1, TRUE);
        }

        res = (bnd != NULL);
    }

    /* allocate indicators array */

    if (res == TRUE)
    {
        if (bnd->buf.inds == NULL)
        {
            bnd->buf.inds = (void *) OCI_MemAlloc(OCI_IPC_INDICATOR_ARRAY, 
                                                  sizeof(sb2), nbelem, TRUE);
        }

        res = (bnd->buf.inds != NULL);
    }

    /* check need for PL/SQL table extra info */

    if ((res == TRUE) && (is_pltbl == TRUE))
    {
        bnd->nbelem = nbelem;
        pnbelem     = &bnd->nbelem;

        /* allocate array of returned codes */

        if (res == TRUE)
        {
            if (bnd->plrcds == NULL)
            {
                bnd->plrcds = (ub2 *) OCI_MemAlloc(OCI_IPC_PLS_RCODE_ARRAY,
                                                   sizeof(ub2), nbelem, TRUE);
            }

            res = (bnd->plrcds != NULL);
        }
    }

    /* for handle based datatypes, we need to allocate an array of handles for
       bind calls because OCILIB uses external arrays of OCILIB Objects */

    if ((res == TRUE) && (mode == OCI_BIND_INPUT))
    {
        if (type != OCI_CDT_RAW      &&
            type != OCI_CDT_LONG     &&
            type != OCI_CDT_CURSOR   &&

#ifndef OCI_CHECK_DATASTRINGS

            type != OCI_CDT_TEXT     &&

#endif
            (type != OCI_CDT_NUMERIC || code == SQLT_VNU)
        )
        {
            bnd->alloc = TRUE;

            if (bnd->buf.data == NULL)
            {
                bnd->buf.data = (void **) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, size, 
                                                       nbelem, TRUE);
            }

            res = (bnd->buf.data != NULL);
        }
        else
            bnd->buf.data = (void **) data;
    }

    /* setup data length array */

    if (res == TRUE && ((is_pltbl == TRUE) || (type == OCI_CDT_RAW)))
    {
        if (bnd->buf.lens == NULL)
        {
            bnd->buf.lens = (void *) OCI_MemAlloc(OCI_IPC_LEN_ARRAY, sizeof(ub2),
                                                  nbelem, TRUE);
        }

        res = (bnd->buf.lens != NULL);

       /* initialize length array with buffer default size */

        if (res == TRUE)
        {
            for (i=0; i < nbelem; i++)
            {
                *(ub2*)(((ub1 *)bnd->buf.lens) + sizeof(ub2) * i) = (ub2) size;
            }
        }

    }

    /* initialize bind object */

    if (res == TRUE)
    {
        /* initialize bind attributes */

        bnd->stmt      = stmt;
        bnd->input     = (void **) data;
        bnd->type      = type;
        bnd->size      = size;
        bnd->code      = (ub2) code;
        bnd->subtype   = (ub1) subtype;

        if (bnd->name == NULL)
        {
            bnd->name = mtsdup(name);
        }

        /* initialize buffer */

        bnd->buf.count   = nbelem;
        bnd->buf.sizelen = sizeof(ub2);

        /* if we bind an OCI_Long or any output bind, we need to change the
           execution mode to provide data at execute time */

        if (bnd->type == OCI_CDT_LONG)
        {
            stmt->long_size = size;
            exec_mode       = OCI_DATA_AT_EXEC;
        }
        else if (mode == OCI_BIND_OUTPUT)
        {
            exec_mode = OCI_DATA_AT_EXEC;
        }
    }

   /* OCI binding */

    if (res == TRUE)
    {
        if (stmt->bind_mode == OCI_BIND_BY_POS)
        {
            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIBindByPos(stmt->stmt, (OCIBind **) &bnd->buf.handle,
                             stmt->con->err, (ub4) index, (void *) bnd->buf.data,
                             bnd->size, bnd->code, bnd->buf.inds,  bnd->buf.lens,
                             bnd->plrcds, (ub4) (is_pltbl == TRUE ? nbelem : 0),
                             pnbelem, exec_mode)
            )
        }
        else
        {
            void * ostr = NULL;
            int osize   = -1;

            ostr = OCI_GetInputMetaString(bnd->name, &osize);

            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIBindByName(stmt->stmt, (OCIBind **) &bnd->buf.handle,
                              stmt->con->err, (OraText *) ostr, (sb4) osize,
                              (void *) bnd->buf.data, bnd->size, bnd->code,
                              bnd->buf.inds, bnd->buf.lens, bnd->plrcds,
                              (ub4) (is_pltbl == TRUE ? nbelem : 0),
                              pnbelem, exec_mode)
            )

            OCI_ReleaseMetaString(ostr);
        }

        if (code == SQLT_NTY || code == SQLT_REF)
        {
            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIBindObject((OCIBind *) bnd->buf.handle, stmt->con->err,
                              (OCIType *) typinf->tdo, (void **) bnd->buf.data,
                              (ub4 *) NULL, (void **) NULL,
                              (ub4 *) bnd->buf.inds)
            )
        }

        if (mode == OCI_BIND_OUTPUT)
        {
            /* register output placeholder */

            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIBindDynamic((OCIBind *) bnd->buf.handle, stmt->con->err,
                               (dvoid *) bnd, OCI_ProcInBind,
                               (dvoid *) bnd, OCI_ProcOutBind)
            )
        }

        /* setup national charset from flag if needed */

        if (
            (
                (bnd->type    == OCI_CDT_LOB) &&
                (bnd->subtype == OCI_NCLOB)
            )
#ifdef OCI_USERDATA_UNICODE
            ||
            (
                (bnd->type          == OCI_CDT_TEXT)           &&
                (OCI_GetVersionConnection(stmt->con) >= OCI_9) &&
                (bnd->buf.lens      == NULL)
            )
#endif
           )
        {
            ub1 csfrm = SQLCS_NCHAR;

            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIAttrSet((dvoid *) bnd->buf.handle, (ub4) OCI_HTYPE_BIND,
                           (dvoid *) &csfrm, (ub4) sizeof(csfrm),
                           (ub4) OCI_ATTR_CHARSET_FORM,  bnd->stmt->con->err)
            )
        }


        if (bnd->type == OCI_CDT_TEXT)
        {
            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIAttrSet((dvoid *) bnd->buf.handle, (ub4) OCI_HTYPE_BIND,
                           (dvoid *) &bnd->size, (ub4) sizeof(bnd->size),
                           (ub4) OCI_ATTR_MAXDATA_SIZE,  bnd->stmt->con->err)
            )
        }

#ifdef OCI_CHARSET_MIXED

        /* setup Unicode mode for user data in mixed builds */

        {
            ub2 csid = OCI_UTF16ID;

            OCI_CALL1
            (
                res, stmt->con, stmt,

                OCIAttrSet((dvoid *) bnd->buf.handle, (ub4) OCI_HTYPE_BIND,
                           (dvoid *) &csid,  (ub4) sizeof(csid),
                           (ub4) OCI_ATTR_CHARSET_ID,  stmt->con->err)
            )
        }

#endif

    }

    /* on success, we :
         - add the bind handle to the bind array
         - add the bind index to the map
    */

    if (res == TRUE)
    {
        if (mode == OCI_BIND_INPUT)
        {
            if (reused == FALSE)
            {
                stmt->ubinds[stmt->nb_ubinds++] = bnd;

                /* for user binds, add a positive index */

                OCI_HashAddInt(stmt->map, name, stmt->nb_ubinds);
            }
        }
        else
        {
            /* for register binds, add a negative index */

            stmt->rbinds[stmt->nb_rbinds++] = bnd;

            index = (int) stmt->nb_rbinds;

            OCI_HashAddInt(stmt->map, name, -index);
        }
    }

    OCI_RESULT(res);

    return res;
}


/* ------------------------------------------------------------------------ *
 * OCI_BindGetIndex
 * ------------------------------------------------------------------------ */

int OCI_BindGetIndex(OCI_Statement *stmt, const mtext *name)
{
    OCI_HashEntry *he = NULL;
    int index         = -1;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, -1);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, -1);

    if (stmt->map != NULL)
    {
        he = OCI_HashLookup(stmt->map, name, FALSE);

        while (he != NULL)
        {
            /* no more entries or key matched => so we got it ! */

            if (he->next == NULL || mtscasecmp(he->key, name) == 0)
            {
                /* in order to sue the same map for user binds and
                   register binds :
                      - user binds are stored as positive values
                      - registers binds are stored as negatives values
                */

                index = he->values->value.num;

                if (index < 0)
                    index = -index;

                break;
            }
        }
    }

    return index;
}

/* ------------------------------------------------------------------------ *
 * OCI_FetchIntoUserVariables
 * ------------------------------------------------------------------------ */

boolean OCI_FetchIntoUserVariables(OCI_Statement *stmt, va_list args)
{
    OCI_Resultset *rs = NULL;
    boolean res = FALSE;
    int i, n;

    /* get resultset */

    rs  = OCI_GetResultset(stmt);

    /* fetch data */

    if (rs != NULL)
        res = OCI_FetchNext(rs);

    if (res == TRUE)
    {
        /* loop on column list for updating user given placeholders */

        for (i = 1, n = OCI_GetColumnCount(rs); i <= n && res == TRUE; i++)
        {
            OCI_Column *col = OCI_GetColumn(rs, i);

            int type = va_arg(args, int);

            switch (type)
            {
                case OCI_ARG_SHORT:
                {
                    short src, *dst;

                    src = OCI_GetShort(rs, i);
                    dst = va_arg(args, short *);

                    if (dst != NULL)
                        *dst = src;

                    break;
                }
                case OCI_ARG_USHORT:
                {
                   unsigned short src, *dst;

                    src = OCI_GetUnsignedShort(rs, i);
                    dst = va_arg(args, unsigned short *);

                    if (dst != NULL)
                        *dst = src;

                    break;
                }
                case OCI_ARG_INT:
                {
                    int src, *dst;

                    src = OCI_GetInt(rs, i);
                    dst = va_arg(args, int *);

                    if (dst != NULL)
                        *dst = src;

                    break;
                }
                case OCI_ARG_UINT:
                {
                   unsigned int src, *dst;

                    src = OCI_GetUnsignedInt(rs, i);
                    dst = va_arg(args, unsigned int *);

                    if (dst != NULL)
                        *dst = src;

                    break;
                }
                case OCI_ARG_BIGINT:
                {
                    big_int src, *dst;

                    src = OCI_GetBigInt(rs, i);
                    dst = va_arg(args, big_int *);

                    if (dst != NULL)
                        *dst = src;

                    break;
                }
                case OCI_ARG_BIGUINT:
                {
                   big_uint src, *dst;

                    src = OCI_GetUnsignedBigInt(rs, i);
                    dst = va_arg(args, big_uint *);

                    if (dst != NULL)
                        *dst = src;

                    break;
                }
                case OCI_ARG_DOUBLE:
                {
                    double src, *dst;

                    src = OCI_GetDouble(rs, i);
                    dst = va_arg(args, double *);

                    if (dst != NULL)
                        *dst = src;

                    break;
                }
                case OCI_ARG_DATETIME:
                {
                    OCI_Date *src, *dst;

                    src = OCI_GetDate(rs, i);
                    dst = (OCI_Date *) va_arg(args, OCI_Date *);

                    if (src != NULL && dst != NULL)
                        res = OCI_DateAssign(dst, src);

                    break;
                }
                case OCI_ARG_TEXT:
                {
                    const dtext *src;
                    dtext *dst;

                    src = OCI_GetString(rs, i);
                    dst = va_arg(args, dtext *);

                    if (dst != NULL)
                        dst[0] = 0;

                    if (dst != NULL && src != NULL)
                        dtscat(dst, src);

                    break;
                }
                case OCI_ARG_RAW:
                {
                    OCI_GetRaw(rs, i, va_arg(args, dtext *), col->bufsize);
                    break;
                }
                case OCI_ARG_LOB:
                {
                    OCI_Lob *src, *dst;

                    src = OCI_GetLob(rs, i);
                    dst = (OCI_Lob *) va_arg(args, OCI_Lob *);

                    if (src != NULL && dst != NULL)
                        res = OCI_LobAssign(dst, src);

                    break;
                }
                case OCI_ARG_FILE:
                {
                    OCI_File *src, *dst;

                    src = OCI_GetFile(rs, i);
                    dst = (OCI_File *) va_arg(args, OCI_File *);

                    if (src != NULL && dst != NULL)
                        res = OCI_FileAssign(dst, src);

                    break;
                }
                case OCI_ARG_TIMESTAMP:
                {
                    OCI_Timestamp *src, *dst;

                    src = OCI_GetTimestamp(rs, i);
                    dst = (OCI_Timestamp *) va_arg(args, OCI_Timestamp *);

                    if (src != NULL && dst != NULL)
                        res = OCI_TimestampAssign(dst, src);

                    break;
                }
                case OCI_ARG_INTERVAL:
                {
                    OCI_Interval *src, *dst;

                    src = OCI_GetInterval(rs, i);
                    dst = (OCI_Interval *) va_arg(args, OCI_Interval *);

                    if (src != NULL && dst != NULL)
                        res =OCI_IntervalAssign(dst, src);

                    break;
                }
                default:
                {
                    OCI_ExceptionMappingArgument(stmt->con, stmt, type);

                    res = FALSE;

                    break;
                }
            }
        }
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_StatementInit
 * ------------------------------------------------------------------------ */

OCI_Statement * OCI_StatementInit(OCI_Connection *con, OCI_Statement **pstmt,
                                  OCIStmt *handle, OCI_Define *def)
{
    OCI_Statement * stmt = NULL;
    boolean res = TRUE;

    OCI_CHECK(pstmt == NULL, NULL);

    if (*pstmt == NULL)
        *pstmt = (OCI_Statement *) OCI_MemAlloc(OCI_IPC_STATEMENT, sizeof(*stmt),
                                              1, TRUE);

    if (*pstmt != NULL)
    {
        stmt = *pstmt;

        stmt->con           = con;
        stmt->stmt          = handle;

        stmt->exec_mode     = OCI_DEFAULT;
        stmt->long_size     = OCI_SIZE_LONG;
        stmt->bind_reuse    = FALSE;
        stmt->bind_mode     = OCI_BIND_BY_NAME;
        stmt->long_mode     = OCI_LONG_EXPLICIT;

        /* reset statement */

        OCI_StatementReset(stmt);

        if (def == NULL)
        {
            /* allocate handle for non fetched lob (temporary lob) */

            stmt->hstate = OCI_OBJECT_ALLOCATED;

            /* allocate handle */

            res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env,
                                                  (dvoid **) (void *) &stmt->stmt,
                                                  (ub4) OCI_HTYPE_STMT,
                                                  (size_t) 0, (dvoid **) NULL));
        }
        else
        {
            stmt->hstate = OCI_OBJECT_FETCHED_CLEAN;
            stmt->status = OCI_STMT_EXECUTED;
            stmt->type   = OCI_CST_SELECT;

            /* not really perfect, but better than nothing */

            if (def->col.name != NULL)
                stmt->sql = mtsdup(def->col.name);
        }

        /* set statement default attributes */

        OCI_SetPrefetchSize(stmt, OCI_PREFETCH_SIZE);
        OCI_SetFetchSize(stmt, OCI_FETCH_SIZE);
    }

    /* check for failure */

    if (res == FALSE)
    {
        OCI_StatementFree(stmt);
        stmt = NULL;
    }

    return stmt;
}

/* ------------------------------------------------------------------------ *
 * OCI_StatementReset
 * ------------------------------------------------------------------------ */

boolean OCI_StatementReset(OCI_Statement *stmt)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    /* free resultsets */

    res = OCI_ReleaseResultsets(stmt);

    /* free in/out binds */

    res = OCI_BindFreeAll(stmt);

    /* free bind map */

    if (stmt->map != NULL)
    {
        OCI_HashFree(stmt->map);
    }

    /* free sql statement */

    OCI_FREE(stmt->sql);

    stmt->rsts        = NULL;
    stmt->sql         = NULL;
    stmt->map         = NULL;

    stmt->status      = OCI_STMT_CLOSED;
    stmt->type        = OCI_UNKNOWN;
    stmt->bind_array  = FALSE;

    stmt->nb_iters     = 1;
    stmt->dynidx       = 0;
    stmt->err_pos      = 0;

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_StatementClose
 * ------------------------------------------------------------------------ */

boolean OCI_StatementClose(OCI_Statement *stmt)
{
    boolean res    = TRUE;
    OCI_Error *err = NULL;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    /* clear statement reference from current error object */

    err = OCI_ErrorGet(FALSE);

    if (err != NULL && err->stmt == stmt)
        err->stmt = NULL;

    /* reset data */

    res = OCI_StatementReset(stmt);

    if (stmt->stmt != NULL && stmt->hstate == OCI_OBJECT_ALLOCATED)
    {
        OCI_HandleFree((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT);
    }

    return res;
}

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_StatementCreate
 * ------------------------------------------------------------------------ */

OCI_Statement * OCI_API OCI_StatementCreate(OCI_Connection *con)
{
    OCI_Statement *stmt = NULL;
    OCI_Item *item      = NULL;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    /* create statement object */

    item = OCI_ListAppend(con->stmts, sizeof(*stmt));

    if (item != NULL)
    {
        stmt = OCI_StatementInit(con, (OCI_Statement **) &item->data, NULL, FALSE);
    }

    OCI_RESULT(stmt != NULL);

    return stmt;
}

/* ------------------------------------------------------------------------ *
 * OCI_StatementFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_StatementFree(OCI_Statement *stmt)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_CHECK_OBJECT_FETCHED(stmt, FALSE);

    res = OCI_StatementClose(stmt);

    OCI_ListRemove(stmt->con->stmts, stmt);

    OCI_FREE(stmt);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ReleaseResultsets
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ReleaseResultsets(OCI_Statement *stmt)
{
    boolean res   = TRUE;
    ub4 i, nb_err = 0;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    if (stmt->rsts != NULL)
    {
        for (i = 0; i  < stmt->nb_rs; i++)
        {
            if (stmt->rsts[i] != NULL)
            {
                if (FALSE == OCI_ResultsetFree(stmt->rsts[i]))
                    nb_err++;
            }
        }

        OCI_FREE(stmt->rsts);
    }

    res = (nb_err == 0);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_Prepare
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_Prepare(OCI_Statement *stmt, const mtext *sql)
{
    boolean res = TRUE;
    void *ostr  = NULL;
    int  osize  = -1;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, sql, FALSE);

    /* reset statement */

    res = OCI_StatementReset(stmt);

    if (res == TRUE)
    {
        /* store SQL */

        stmt->sql = mtsdup(sql);

        ostr = OCI_GetInputMetaString(stmt->sql, &osize);

        /* prepare SQL */

        OCI_CALL1
        (
            res, stmt->con, stmt,

            OCIStmtPrepare(stmt->stmt,stmt->con->err, (OraText *) ostr,
                           (ub4) osize, (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT)
        )

        OCI_ReleaseMetaString(ostr);

        /* get statement type */

        OCI_CALL1
        (
            res, stmt->con, stmt,

            OCIAttrGet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                       (dvoid *) &stmt->type, (ub4 *) NULL,
                       (ub4) OCI_ATTR_STMT_TYPE, stmt->con->err)
        )
    }

    /* update statement status */

    if (res == TRUE)
        stmt->status = OCI_STMT_PREPARED;

    OCI_RESULT(res);

   return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_Execute
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_Execute(OCI_Statement *stmt)
{
    boolean res   = TRUE;
    sword status  = OCI_SUCCESS;
    ub4 iters     = 0;
    ub4 mode      = OCI_DEFAULT;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);
    OCI_CHECK_STMT_STATUS(stmt, OCI_STMT_CLOSED, FALSE);

    /* set up iters and mode values for execution */

    if (stmt->type == OCI_CST_SELECT)
        mode  = stmt->exec_mode;
    else
        iters = stmt->nb_iters;

    /* check bind objects for updating their null indicator status */

    res = OCI_BindCheck(stmt);

    /* free previous resulsets in case of re-execution of the same SQL order */

   if (res == TRUE)
       res = OCI_ReleaseResultsets(stmt);

    /* Oracle execute call */

    status = OCIStmtExecute(stmt->con->cxt, stmt->stmt, stmt->con->err, iters,
                            (ub4) 0, (OCISnapshot *) NULL, (OCISnapshot *) NULL,
                            mode);

    /* reset input binds indicators status even if execution failed */

    OCI_BindReset(stmt);

    /* check result */

    res = ((status == OCI_SUCCESS) || (status == OCI_NEED_DATA));

    /* update status on success */

    if (res == TRUE)
    {
        stmt->status = OCI_STMT_EXECUTED;

        /* commit if necessary */

        if (stmt->con->autocom == TRUE)
            OCI_Commit(stmt->con);
    }
    else
    {
        /* get parse error position type */

        /* (one of the rare OCI call not enclosed with a OCI_CALLX macro ...) */


        OCIAttrGet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                   (dvoid *) &stmt->err_pos, (ub4 *) NULL,
                   (ub4) OCI_ATTR_PARSE_ERROR_OFFSET, stmt->con->err);

        /* raise exception */

        OCI_ExceptionOCI(stmt->con->err, stmt->con, stmt);
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ExecuteStmt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ExecuteStmt(OCI_Statement *stmt, const mtext *sql)
{
    return (OCI_Prepare(stmt, sql) && OCI_Execute(stmt));
}

/* ------------------------------------------------------------------------ *
 * OCI_PrepareFmt
 * ------------------------------------------------------------------------ */

boolean OCI_PrepareFmt(OCI_Statement *stmt, const mtext *sql, ...)
{
    boolean res    = FALSE;
    mtext *sql_fmt = NULL;
    va_list args;
    int size;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, sql, FALSE);

    /* first, get buffer size */

    va_start(args, sql);

    size = OCI_ParseSqlFmt(stmt, NULL, sql, &args);

    va_end(args);

    if (size > 0)
    {
        /* allocate buffer */

        sql_fmt = (mtext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(mtext), (size+1),
                                         TRUE);

        if (sql_fmt != NULL)
        {
            /* format buffer */

            va_start(args, sql);

            if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
            {
               /* parse buffer */

               res = OCI_Prepare(stmt, sql_fmt);
            }

            va_end(args);

            OCI_FREE(sql_fmt);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ExecuteStmtFmt
 * ------------------------------------------------------------------------ */

boolean OCI_ExecuteStmtFmt(OCI_Statement *stmt, const mtext *sql, ...)
{
    boolean res    = FALSE;
    mtext *sql_fmt = NULL;
    va_list args;
    int size;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, sql, FALSE);

    /* first, get buffer size */

    va_start(args, sql);

    size = OCI_ParseSqlFmt(stmt, NULL, sql, &args);

    va_end(args);

    if (size > 0)
    {
        /* allocate buffer */

        sql_fmt = (mtext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(mtext), (size+1),
                                         TRUE);

        if (sql_fmt != NULL)
        {
            /* format buffer */

            va_start(args, sql);

            if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
            {
                /* prepare and execute SQL buffer */

                res = (OCI_Prepare(stmt, sql_fmt) &&  OCI_Execute(stmt));
            }

            va_end(args);

            OCI_FREE(sql_fmt);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_Immediate
 * ------------------------------------------------------------------------ */

boolean OCI_Immediate(OCI_Connection *con, const mtext *sql, ...)
{
    OCI_Statement *stmt = NULL;
    boolean res = FALSE;
    va_list args;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, sql, FALSE);

    /* First, execute SQL */

    stmt = OCI_StatementCreate(con);

    if ((stmt != NULL) &&OCI_ExecuteStmt(stmt, sql))
    {
        /* get resultset and set up variables */

        if (OCI_GetStatementType(stmt) == OCI_CST_SELECT)
        {
            va_start(args, sql);

            res = OCI_FetchIntoUserVariables(stmt, args);

            va_end(args);
        }

        OCI_StatementFree(stmt);
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ImmediateFmt
 * ------------------------------------------------------------------------ */

boolean OCI_ImmediateFmt(OCI_Connection *con, const mtext *sql, ...)
{
    OCI_Statement *stmt = NULL;
    mtext *sql_fmt = NULL;
    boolean res = FALSE;
    va_list args;
    int size;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, sql, FALSE);

    stmt = OCI_StatementCreate(con);

    if (stmt != NULL)
    {
        /* first, get buffer size */

        va_start(args, sql);

        size = OCI_ParseSqlFmt(stmt, NULL, sql, &args);

        va_end(args);

        if (size > 0)
        {
            /* allocate buffer */

            sql_fmt = (mtext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(mtext),
                                             (size+1), TRUE);

            if (sql_fmt != NULL)
            {
                /* format buffer */

                va_start(args, sql);

                if (OCI_ParseSqlFmt(stmt, sql_fmt, sql, &args) > 0)
                {
                    /* prepare and execute SQL buffer */

                    res = (OCI_Prepare(stmt, sql_fmt) &&  OCI_Execute(stmt));

                    /* get resultset and set up variables */

                    if (res && (OCI_GetStatementType(stmt) == OCI_CST_SELECT))
                    {
                        res = OCI_FetchIntoUserVariables(stmt, args);
                    }
                }

                va_end(args);

                OCI_FREE(sql_fmt);
            }
        }

        OCI_StatementFree(stmt);
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArraySetSize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArraySetSize(OCI_Statement *stmt, unsigned int size)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_CHECK_MIN(stmt->con, stmt, size, 1, FALSE);

    OCI_CHECK_STMT_STATUS(stmt, OCI_STMT_CLOSED, FALSE);

    /* if the statements already has binds, we need to check if the new size is
       not greater than the initial size
    */

    if ((stmt->nb_ubinds > 0) && (size > stmt->ubinds[0]->buf.count))
    {
        OCI_ExceptionBindArraySize(stmt, stmt->ubinds[0]->buf.count, 
                                         stmt->nb_iters, size);

        res = FALSE;
    }
    else
    {
        stmt->nb_iters   = size;
        stmt->bind_array = TRUE;
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayGetSize
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_BindArrayGetSize(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_RESULT(TRUE);

    return stmt->nb_iters;
}

/* ------------------------------------------------------------------------ *
 * OCI_AllowRebinding
 * ------------------------------------------------------------------------ */

OCI_EXPORT boolean OCI_API OCI_AllowRebinding(OCI_Statement *stmt, boolean value)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_RESULT(TRUE);

    stmt->bind_reuse = value;

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindShort(OCI_Statement *stmt, const mtext *name, short *data)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_SHORT);

    return OCI_BindData(stmt, data, sizeof(short), name, OCI_CDT_NUMERIC,
                        SQLT_INT, OCI_BIND_INPUT, OCI_NUM_SHORT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfShorts
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfShorts(OCI_Statement *stmt, const mtext *name,
                                    short *data, unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_SHORT);

    return OCI_BindData(stmt, data, sizeof(short), name, OCI_CDT_NUMERIC,
                        SQLT_INT, OCI_BIND_INPUT, OCI_NUM_SHORT, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindUnsignedShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindUnsignedShort(OCI_Statement *stmt, const mtext *name,
                                    unsigned short *data)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_SHORT);

    return OCI_BindData(stmt, data, sizeof(unsigned short), name, OCI_CDT_NUMERIC,
                        SQLT_UIN, OCI_BIND_INPUT, OCI_NUM_USHORT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfUnsignedShorts
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfUnsignedShorts(OCI_Statement *stmt,
                                            const mtext *name,
                                            unsigned short *data,
                                            unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_SHORT);

    return OCI_BindData(stmt, data, sizeof(unsigned short), name, OCI_CDT_NUMERIC,
                        SQLT_UIN, OCI_BIND_INPUT, OCI_NUM_USHORT, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindInt(OCI_Statement *stmt, const mtext *name, int *data)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_INT);

    return OCI_BindData(stmt, data, sizeof(int), name, OCI_CDT_NUMERIC,
                        SQLT_INT, OCI_BIND_INPUT, OCI_NUM_INT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfInts
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfInts(OCI_Statement *stmt, const mtext *name,
                                    int *data, unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_INT);

   return OCI_BindData(stmt, data, sizeof(int), name, OCI_CDT_NUMERIC,
                       SQLT_INT, OCI_BIND_INPUT, OCI_NUM_INT, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindUnsignedInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindUnsignedInt(OCI_Statement *stmt, const mtext *name,
                                    unsigned int *data)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_INT);

    return OCI_BindData(stmt, data, sizeof(unsigned int), name, OCI_CDT_NUMERIC,
                        SQLT_UIN, OCI_BIND_INPUT, OCI_NUM_UINT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfUnsignedInts
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfUnsignedInts(OCI_Statement *stmt, const mtext *name,
                                            unsigned int *data, unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_INT);

    return OCI_BindData(stmt, data, sizeof(unsigned int), name, OCI_CDT_NUMERIC,
                        SQLT_UIN, OCI_BIND_INPUT, OCI_NUM_UINT, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindBigInt(OCI_Statement *stmt, const mtext *name,
                               big_int *data)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_BIGINT);

    return OCI_BindData(stmt, data, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_INPUT, OCI_NUM_BIGINT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfBigInts
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfBigInts(OCI_Statement *stmt, const mtext *name,
                                       big_int *data, unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_BIGINT);

    return OCI_BindData(stmt, data, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_INPUT, OCI_NUM_BIGINT, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindUnsignedBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindUnsignedBigInt(OCI_Statement *stmt, const mtext *name,
                                      big_uint *data)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_BIGINT);

    return OCI_BindData(stmt, data, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_INPUT, OCI_NUM_BIGUINT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfUnsignedInts
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfUnsignedBigInts(OCI_Statement *stmt,
                                               const mtext *name,
                                               big_uint *data,
                                               unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_BIGINT);

    return OCI_BindData(stmt, data, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_INPUT, OCI_NUM_BIGUINT, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindString
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindString(OCI_Statement *stmt, const mtext *name,
                               dtext *data, unsigned int len)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_STRING);

    if (len == 0 || len == UINT_MAX)
        len = (int) dtslen(data);

    return OCI_BindData(stmt, data, (len + 1) * sizeof(odtext), name, OCI_CDT_TEXT,
                        SQLT_STR, OCI_BIND_INPUT, 0, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfStrings
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfStrings(OCI_Statement *stmt, const mtext *name,
                                       dtext *data, unsigned int len,
                                       unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_STRING);

    OCI_CHECK_MIN(stmt->con, stmt, len, 1, FALSE);

    return OCI_BindData(stmt, data, (len + 1) * sizeof(odtext), name, OCI_CDT_TEXT,
                        SQLT_STR, OCI_BIND_INPUT, 0, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindRaw
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindRaw(OCI_Statement *stmt, const mtext *name, void *data,
                               unsigned int len)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_VOID);

    OCI_CHECK_MIN(stmt->con, stmt, len, 1, FALSE);

    return OCI_BindData(stmt, data, len, name, OCI_CDT_RAW,
                        SQLT_BIN, OCI_BIND_INPUT, 0, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfRaws
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfRaws(OCI_Statement *stmt, const mtext *name,
                                       void  *data, unsigned int len,
                                       unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_VOID);

    OCI_CHECK_MIN(stmt->con, stmt, len, 1, FALSE);

    return OCI_BindData(stmt, data, len, name, OCI_CDT_RAW,
                        SQLT_BIN, OCI_BIND_INPUT, 0, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindDouble
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindDouble(OCI_Statement *stmt, const mtext *name,
                               double *data)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_DOUBLE);

    return OCI_BindData(stmt, data, sizeof(double), name, OCI_CDT_NUMERIC,
                        SQLT_FLT, OCI_BIND_INPUT, 0, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfDoubles
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfDoubles(OCI_Statement *stmt, const mtext *name,
                                       double *data, unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_DOUBLE);

    return OCI_BindData(stmt, data, sizeof(double), name, OCI_CDT_NUMERIC,
                        SQLT_FLT, OCI_BIND_INPUT, 0, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindDate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindDate(OCI_Statement *stmt, const mtext *name,
                             OCI_Date *data)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_DATE);

    return OCI_BindData(stmt, data, sizeof(OCIDate), name, OCI_CDT_DATETIME,
                        SQLT_ODT, OCI_BIND_INPUT, 0, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfDates
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfDates(OCI_Statement *stmt, const mtext *name,
                                     OCI_Date **data, unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_DATE);

    return OCI_BindData(stmt, data, sizeof(OCIDate), name, OCI_CDT_DATETIME,
                        SQLT_ODT, OCI_BIND_INPUT, 0, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindTimestamp
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindTimestamp(OCI_Statement *stmt, const mtext *name,
                                  OCI_Timestamp *data)
{
    int code    = 0;
    boolean res = FALSE;

    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_TIMESTAMP);

    OCI_CHECK_TIMESTAMP_ENABLED(stmt->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* map oracle internal type */

    if (data->type == OCI_TIMESTAMP_TZ)
        code = SQLT_TIMESTAMP_TZ;
    else if (data->type == OCI_TIMESTAMP_LTZ)
        code = SQLT_TIMESTAMP_LTZ;
    else
        code = SQLT_TIMESTAMP;

    res = OCI_BindData(stmt, data, sizeof(OCIDateTime *), name, OCI_CDT_TIMESTAMP,
                       code, OCI_BIND_INPUT, data->type, NULL, 0);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(code);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfTimestamps
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfTimestamps(OCI_Statement *stmt, const mtext *name,
                                          OCI_Timestamp **data,
                                          unsigned int type,
                                          unsigned int nbelem)
{
    unsigned int code    = 0;
    boolean res          = FALSE;

    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_TIMESTAMP);

    OCI_CHECK_TIMESTAMP_ENABLED(stmt->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* map oracle internal type */

    if (type == OCI_TIMESTAMP_TZ)
        code = SQLT_TIMESTAMP_TZ;
    else if (type == OCI_TIMESTAMP_LTZ)
        code = SQLT_TIMESTAMP_LTZ;
    else
        code = SQLT_TIMESTAMP;

    res =  OCI_BindData(stmt, data, sizeof(OCIDateTime *), name, OCI_CDT_TIMESTAMP,
                        code, OCI_BIND_INPUT, type, NULL, nbelem);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(code);
    OCI_NOT_USED(nbelem);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindInterval
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindInterval(OCI_Statement *stmt, const mtext *name,
                                  OCI_Interval *data)
{
    int code    = 0;
    boolean res = FALSE;

    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_INTERVAL);

    OCI_CHECK_INTERVAL_ENABLED(stmt->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* map oracle internal type */

    if (data->type == OCI_INTERVAL_YM)
        code = SQLT_INTERVAL_YM;
    else if (data->type == OCI_INTERVAL_DS)
        code = SQLT_INTERVAL_DS;

    res = OCI_BindData(stmt, data, sizeof(OCIInterval *), name, OCI_CDT_INTERVAL,
                       code, OCI_BIND_INPUT, data->type, NULL, 0);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfIntervals
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfIntervals(OCI_Statement *stmt, const mtext *name,
                                         OCI_Interval **data,
                                         unsigned int type,
                                         unsigned int nbelem)
{
    unsigned int code = 0;
    boolean res       = FALSE;

    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_INTERVAL);

    OCI_CHECK_INTERVAL_ENABLED(stmt->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* map oracle internal type */

    if (type == OCI_INTERVAL_YM)
        code = SQLT_INTERVAL_YM;
    else if (type == OCI_INTERVAL_DS)
        code = SQLT_INTERVAL_DS;

    res = OCI_BindData(stmt, data, sizeof(OCIInterval *), name, OCI_CDT_INTERVAL,
                       code, OCI_BIND_INPUT, type, NULL, nbelem);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(code);
    OCI_NOT_USED(nbelem);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindObject
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindObject(OCI_Statement *stmt, const mtext *name,
                               OCI_Object *data)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_OBJECT);

    return OCI_BindData(stmt, data, sizeof(void*), name, OCI_CDT_OBJECT,
                        SQLT_NTY, OCI_BIND_INPUT, 0, data->typinf, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfObjects
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfObjects(OCI_Statement *stmt, const mtext *name,
                                       OCI_Object **data, OCI_TypeInfo *typinf,
                                       unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_OBJECT);

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, FALSE);

    return OCI_BindData(stmt, data, sizeof(void *), name, OCI_CDT_OBJECT,
                        SQLT_NTY, OCI_BIND_INPUT, 0, typinf, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindLob
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindLob(OCI_Statement *stmt, const mtext *name,
                            OCI_Lob *data)
{
    int code = 0;

    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_LOB);

    /* map oracle internal type */

    if (data->type == OCI_CLOB || data->type == OCI_NCLOB)
        code = SQLT_CLOB;
    else
        code = SQLT_BLOB;

    return OCI_BindData(stmt, data, sizeof(OCILobLocator*), name, OCI_CDT_LOB,
                        code, OCI_BIND_INPUT, data->type, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfLobs
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfLobs(OCI_Statement *stmt, const mtext *name,
                                    OCI_Lob **data, unsigned int type,
                                    unsigned int nbelem)
{
    unsigned int code = 0;

    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_LOB);

    /* map oracle internal type */

    if (type == OCI_CLOB || type == OCI_NCLOB)
        code = SQLT_CLOB;
    else
        code = SQLT_BLOB;

    return OCI_BindData(stmt, data, sizeof(OCILobLocator*), name, OCI_CDT_LOB,
                        code, OCI_BIND_INPUT, type, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindFile
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindFile(OCI_Statement *stmt, const mtext *name,
                             OCI_File *data)
{
    int code = 0;

    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_FILE);

    /* map oracle internal type */

    if (data->type == OCI_CFILE)
        code = SQLT_CFILE;
    else
        code = SQLT_BFILE;

    return OCI_BindData(stmt, data, sizeof(OCILobLocator*), name, OCI_CDT_FILE,
                        code, OCI_BIND_INPUT, data->type, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfFiles
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfFiles(OCI_Statement *stmt, const mtext *name,
                                     OCI_File **data, unsigned int type,
                                     unsigned int nbelem)
{
    unsigned int code = 0;

    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_FILE);

    /* map oracle internal type */

    if (type == OCI_CFILE)
        code = SQLT_CFILE;
    else
        code = SQLT_BFILE;

    return OCI_BindData(stmt, data, sizeof(OCILobLocator*), name, OCI_CDT_FILE,
                        code, OCI_BIND_INPUT, type, NULL, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindRef
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindRef(OCI_Statement *stmt, const mtext *name, OCI_Ref *data)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_REF);

    return OCI_BindData(stmt, data, sizeof(OCIRef *), name, OCI_CDT_REF,
                        SQLT_REF, OCI_BIND_INPUT, 0, data->typinf, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfRefs
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfRefs(OCI_Statement *stmt, const mtext *name,
                                     OCI_Ref **data, OCI_TypeInfo *typinf,
                                     unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_REF);

    return OCI_BindData(stmt, data, sizeof(OCIRef *), name, OCI_CDT_REF,
                        SQLT_REF, OCI_BIND_INPUT, 0, typinf, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindColl
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindColl(OCI_Statement *stmt, const mtext *name,
                             OCI_Coll *data)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_COLLECTION);

    return OCI_BindData(stmt, data, sizeof(OCIColl*), name,
                        OCI_CDT_COLLECTION, SQLT_NTY, OCI_BIND_INPUT, 0,
                        data->typinf, 0);
}


/* ------------------------------------------------------------------------ *
 * OCI_BindArrayOfColls
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindArrayOfColls(OCI_Statement *stmt, const mtext *name,
                                     OCI_Coll **data, OCI_TypeInfo *typinf,
                                     unsigned int nbelem)
{
    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_COLLECTION);

    return OCI_BindData(stmt, data, sizeof(OCIColl*), name,
                        OCI_CDT_COLLECTION, SQLT_NTY, OCI_BIND_INPUT, 0,
                        typinf, nbelem);
}

/* ------------------------------------------------------------------------ *
 * OCI_BindStatement
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindStatement(OCI_Statement *stmt, const mtext *name,
                                  OCI_Statement *data)
{
    boolean res = FALSE;

    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_STATEMENT);

    OCI_StatementReset(data);

    res = OCI_BindData(stmt, &data->stmt, sizeof(OCIStmt*), name, OCI_CDT_CURSOR,
                      SQLT_RSET, OCI_BIND_INPUT, 0, NULL, 0);

    if (res == TRUE)
    {
        /* Once stmt is executed, Oracle provides a statement handle
           ready to be fetched  */

        data->status  = OCI_STMT_EXECUTED;
        data->type    = OCI_CST_SELECT;
    }

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_BindLong
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_BindLong(OCI_Statement *stmt, const mtext *name,
                             OCI_Long *data, unsigned int size)
{
    int code = 0;

    OCI_CHECK_BIND_CALL(stmt, name, data, OCI_IPC_LONG);

    /* map oracle internal type */

    if (data->type == OCI_CLONG)
        code = SQLT_LNG;
    else
        code = SQLT_LBI;

    if (data->type == OCI_CLONG)
        size *= sizeof(dtext);

    return OCI_BindData(stmt, data, size, name, OCI_CDT_LONG,
                        code, OCI_BIND_INPUT, data->type, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterShort(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_SHORT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterUnsignedShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterUnsignedShort(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_USHORT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterInt(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_INT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterUnsignedInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterUnsignedInt(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_UINT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterBigInt(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_BIGINT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterUnsignedBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterUnsignedBigInt(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_BIGUINT, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterString
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterString(OCI_Statement *stmt, const mtext *name,
                                   unsigned int len)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    OCI_CHECK_MIN(stmt->con, stmt, len, 1, FALSE);

    return OCI_BindData(stmt, NULL, (len+1)*sizeof(odtext), name, OCI_CDT_TEXT,
                        SQLT_STR, OCI_BIND_OUTPUT, 0, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterRaw
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterRaw(OCI_Statement *stmt, const mtext *name,
                                unsigned int len)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    OCI_CHECK_MIN(stmt->con, stmt, len, 1, FALSE);

    return OCI_BindData(stmt, NULL, len, name, OCI_CDT_RAW,
                        SQLT_BIN, OCI_BIND_OUTPUT, 0, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterDouble
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterDouble(OCI_Statement *stmt, const mtext *name)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    return OCI_BindData(stmt, NULL, sizeof(OCINumber), name, OCI_CDT_NUMERIC,
                        SQLT_VNU, OCI_BIND_OUTPUT, OCI_NUM_DOUBLE, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterDate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterDate(OCI_Statement *stmt, const mtext *name)
{
    int code = SQLT_ODT;
    int size = sizeof(OCIDate);

    OCI_CHECK_REGISTER_CALL(stmt, name);

    /* versions of OCI (< 10.2) crashes if SQLT_ODT is passed for output
       data with returning clause.
       It's an Oracle known bug #3269146 */

    if (OCI_GetVersionConnection(stmt->con) < OCI_11)
    {
        code = SQLT_DAT;
        size = 7;
    }

    return OCI_BindData(stmt, NULL, size, name, OCI_CDT_DATETIME,
                        code, OCI_BIND_OUTPUT, 0, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterTimestamp
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterTimestamp(OCI_Statement *stmt, const mtext *name,
                                      unsigned int type)
{
    int code    = 0;
    boolean res = FALSE;

    OCI_CHECK_REGISTER_CALL(stmt, name);

    OCI_CHECK_TIMESTAMP_ENABLED(stmt->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* map oracle internal type */

    if (type == OCI_TIMESTAMP_TZ)
        code = SQLT_TIMESTAMP_TZ;
    else if (type == OCI_TIMESTAMP_LTZ)
        code = SQLT_TIMESTAMP_LTZ;
    else
        code = SQLT_TIMESTAMP;

    res = OCI_BindData(stmt, NULL, sizeof(OCIDateTime *), name, OCI_CDT_TIMESTAMP,
                       code, OCI_BIND_OUTPUT, type, NULL, 0);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterInterval
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterInterval(OCI_Statement *stmt, const mtext *name,
                                     unsigned int type)
{
    unsigned int code    = 0;
    boolean res          = FALSE;

    OCI_CHECK_REGISTER_CALL(stmt, name);

    OCI_CHECK_INTERVAL_ENABLED(stmt->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* map oracle internal type */

    if (type == OCI_INTERVAL_YM)
        code = SQLT_INTERVAL_YM;
    else if (type == OCI_INTERVAL_DS)
        code = SQLT_INTERVAL_DS;

    res = OCI_BindData(stmt, NULL, sizeof(OCIInterval *), name, OCI_CDT_INTERVAL,
                       code, OCI_BIND_OUTPUT, type, NULL, 0);

#else

    OCI_NOT_USED(name);
    OCI_NOT_USED(type);
    OCI_NOT_USED(code);

#endif

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterObject
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterObject(OCI_Statement *stmt, const mtext *name,
                                   OCI_TypeInfo *typinf)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, FALSE);

    return OCI_BindData(stmt, NULL, sizeof(OCIInterval *), name, OCI_CDT_OBJECT,
                        SQLT_NTY, OCI_BIND_OUTPUT, 0, typinf, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterLob
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterLob(OCI_Statement *stmt, const mtext *name,
                                unsigned int type)
{
    unsigned int code = 0;

    OCI_CHECK_REGISTER_CALL(stmt, name);

    /* map oracle internal type */

    if (type == OCI_CLOB || type == OCI_NCLOB)
        code = SQLT_CLOB;
    else
        code = SQLT_BLOB;

    return OCI_BindData(stmt, NULL, sizeof(OCILobLocator*), name, OCI_CDT_LOB,
                        code, OCI_BIND_OUTPUT, type, NULL, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_RegisterFile
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterFile(OCI_Statement *stmt, const mtext *name,
                                 unsigned int type)
{
    unsigned int code;

    OCI_CHECK_REGISTER_CALL(stmt, name);

    /* map oracle internal type */

    if (type == OCI_CFILE)
        code = SQLT_CFILE;
    else
        code = SQLT_BFILE;

    return OCI_BindData(stmt, NULL, sizeof(OCILobLocator*), name, OCI_CDT_FILE,
                        code, OCI_BIND_OUTPUT, type, NULL, 0);
}


/* ------------------------------------------------------------------------ *
 * OCI_RegisterRef
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_RegisterRef(OCI_Statement *stmt, const mtext *name,
                                OCI_TypeInfo *typinf)
{
    OCI_CHECK_REGISTER_CALL(stmt, name);

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, FALSE);

    return OCI_BindData(stmt, NULL, sizeof(OCIRef *), name, OCI_CDT_REF,
                        SQLT_REF, OCI_BIND_OUTPUT, 0, typinf, 0);
}

/* ------------------------------------------------------------------------ *
 * OCI_SetNullAtPos
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetNullAtPos(OCI_Statement *stmt, unsigned int index,
                                 unsigned int position)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_CHECK_BOUND(stmt->con, index   , 1, stmt->nb_ubinds, FALSE);
    OCI_CHECK_BOUND(stmt->con, position, 1, stmt->ubinds[index-1]->buf.count, FALSE);


    if (stmt->ubinds[index-1]->buf.inds != NULL)
        ((sb2*) stmt->ubinds[index-1]->buf.inds)[position-1] = -1;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetNullAtPos2
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetNullAtPos2(OCI_Statement *stmt, const mtext *name,
                                  unsigned int position)
{
    return OCI_SetNullAtPos(stmt, OCI_BindGetIndex(stmt, name), position);
}

/* ------------------------------------------------------------------------ *
 * OCI_SetNull
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetNull(OCI_Statement *stmt, unsigned int index)
{
  return OCI_SetNullAtPos(stmt, index, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_SetNullAtPos2
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetNull2(OCI_Statement *stmt, const mtext *name)
{
    return OCI_SetNullAtPos2(stmt, name, 1);
}

/* ------------------------------------------------------------------------ *
 * OCI_GetStatementType
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetStatementType(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return stmt->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetFetchMode
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetFetchMode(OCI_Statement *stmt, unsigned int mode)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_CHECK_SCROLLABLE_CURSOR_ENABLED(stmt->con, FALSE);

    stmt->exec_mode = mode;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetFetchMode
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetFetchMode(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, OCI_UNKNOWN);

    OCI_CHECK_SCROLLABLE_CURSOR_ENABLED(stmt->con, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return stmt->exec_mode;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetBindMode
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetBindMode(OCI_Statement *stmt, unsigned int mode)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    stmt->bind_mode = mode;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetBindMode
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetBindMode(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->bind_mode;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetFetchSize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetFetchSize(OCI_Statement *stmt, unsigned int size)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    OCI_CHECK_MIN(stmt->con, stmt, size, 1, FALSE);

    stmt->fetch_size = size;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetFetchSize
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetFetchSize(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->fetch_size;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetPrefetchSize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetPrefetchSize(OCI_Statement *stmt, unsigned int size)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt,  FALSE);

    OCI_CALL1
    (
        res, stmt->con, stmt,

        OCIAttrSet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                   (dvoid *) &stmt->prefetch_size,
                   (ub4) sizeof(stmt->prefetch_size),
                   (ub4) OCI_ATTR_PREFETCH_ROWS, stmt->con->err)
    )

    if (res == TRUE)
        stmt->prefetch_size = size;

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetPrefetchSize
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetPrefetchSize(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->prefetch_size;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetPrefetchMemory
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetPrefetchMemory(OCI_Statement *stmt, unsigned int size)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt,  FALSE);

    OCI_CALL1
    (
        res, stmt->con, stmt,

        OCIAttrSet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                   (dvoid *) &stmt->prefetch_mem,
                   (ub4) sizeof(stmt->prefetch_mem),
                   (ub4) OCI_ATTR_PREFETCH_MEMORY, stmt->con->err)
    )

    if (res == TRUE)
        stmt->prefetch_mem = size;

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetPrefetchMemory
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetPrefetchMemory(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->prefetch_mem;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetLongMaxSize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetLongMaxSize(OCI_Statement *stmt, unsigned int size)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt,  FALSE);

    OCI_CHECK_MIN(stmt->con, stmt, size, 1, FALSE);

    stmt->long_size = size;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetLongMaxSize
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetLongMaxSize(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->long_size;
}

/* ------------------------------------------------------------------------ *
 * OCI_SetLongMode
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SetLongMode(OCI_Statement *stmt, unsigned int mode)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);

    stmt->long_mode = (ub1) mode;

    OCI_RESULT(TRUE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetLongMode
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetLongMode(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->long_mode;
}

/* ------------------------------------------------------------------------ *
 * OCI_StatementGetConnection
 * ------------------------------------------------------------------------ */

OCI_Connection * OCI_API OCI_StatementGetConnection(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->con;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetSql
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_GetSql(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, NULL);

    OCI_RESULT(TRUE);

    return stmt->sql;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetSqlErrorPos
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetSqlErrorPos(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return stmt->err_pos;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetAffecteddRows
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetAffectedRows(OCI_Statement *stmt)
{
    boolean res = TRUE;
    ub4 count   = 0;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt,  FALSE);

    OCI_CALL1
    (
        res, stmt->con, stmt,

        OCIAttrGet((dvoid *) stmt->stmt, (ub4) OCI_HTYPE_STMT,
                   (void *) &count, (ub4 *) NULL, (ub4) OCI_ATTR_ROW_COUNT,
                   stmt->con->err)
    )

    OCI_RESULT(res);

    return count;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetBindCount
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_GetBindCount(OCI_Statement *stmt)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, 0);

    OCI_RESULT(TRUE);

    return (unsigned int) stmt->nb_ubinds;
}

/* ------------------------------------------------------------------------ *
 * OCI_GetBind
 * ------------------------------------------------------------------------ */

OCI_Bind * OCI_API OCI_GetBind(OCI_Statement *stmt, unsigned int index)
{
    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, NULL);
    OCI_CHECK_BOUND(stmt->con, index, 1, stmt->nb_ubinds, NULL);

    OCI_RESULT(TRUE);

    return stmt->ubinds[index-1];
}

/* ------------------------------------------------------------------------ *
 * OCI_GetBind2
 * ------------------------------------------------------------------------ */

OCI_Bind * OCI_API OCI_GetBind2(OCI_Statement *stmt, const mtext *name)
{
    OCI_Bind *bnd = NULL;
    int index     = -1;

    OCI_CHECK_PTR(OCI_IPC_STATEMENT, stmt, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, FALSE);

    index =  OCI_BindGetIndex(stmt, name);

    if (index > 0)
        bnd = stmt->ubinds[index-1];

    OCI_RESULT(bnd != NULL);

    return bnd;
}
