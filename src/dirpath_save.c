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
 * $Id: dirpath.c, v 3.2.0 2009/01/23 21:45 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_DirPathCreate
 * ------------------------------------------------------------------------ */

OCI_DirPath * OCI_API OCI_DirPathCreate(OCI_Connection *con,  mtext *schema,
                                        mtext *table, mtext *partition,
                                        unsigned int nb_cols,
                                        unsigned int nb_rows)
{
    OCI_DirPath *dp = NULL;

    void *ostr  = NULL;
    int osize   = -1;

    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con,   NULL);
    OCI_CHECK_PTR(OCI_IPC_STRING,     table, NULL);

    /* allocate direct path structure  */

    dp = (OCI_DirPath *) OCI_MemAlloc(OCI_IPC_DIRPATH, sizeof(*dp), 1, TRUE);

    if (dp != NULL)
    {
        dp->con       = con;
        dp->status    = OCI_DPS_ALLOCATED;
        dp->schema    = mtsdup(schema);
        dp->table     = mtsdup(table);
        dp->partition = mtsdup(partition);
        dp->nb_rows   = (ub4) nb_rows;
        dp->nb_cols   = (ub4) nb_cols;
        dp->nb_cur    = (ub4) dp->nb_rows;
        dp->err_col   = -1;
        dp->nb_prcsd  = 0;

        /* allocates direct context handle */

        if (res == TRUE)
        {
            res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) OCILib.env,
                                                  (dvoid **) (void *) &dp->ctx,
                                                  (ub4) OCI_HTYPE_DIRPATH_CTX,
                                                  (size_t) 0, (dvoid **) NULL));
        }

        /* set table name attribute */

        if (res == TRUE)
        {
            osize = -1;
            ostr  = OCI_GetInputMetaString(dp->table, &osize);

            OCI_CALL2
            (
                res, dp->con,

                OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                           (dvoid *) ostr, (ub4) osize,
                           (ub4) OCI_ATTR_NAME, dp->con->err)
            )

            OCI_ReleaseMetaString(ostr);
        }

        /* set schema name attribute */

        if ((res == TRUE) && (dp->schema != NULL) && (dp->schema[0] != 0))
        {
            osize = -1;
            ostr  = OCI_GetInputMetaString(dp->schema, &osize);

            OCI_CALL2
            (
                res, dp->con,

                OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                           (dvoid *) ostr, (ub4) osize,
                           (ub4) OCI_ATTR_SCHEMA_NAME, dp->con->err)
            )

            OCI_ReleaseMetaString(ostr);
        }

        /* set partition name attribute */

        if ((res == TRUE) && (dp->partition != NULL) && (dp->partition[0] != 0))
        {
            osize = -1;
            ostr  = OCI_GetInputMetaString(dp->schema, &osize);

            OCI_CALL2
            (
                res, dp->con,

                OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                           (dvoid *) ostr, (ub4) osize,
                           (ub4) OCI_ATTR_SUB_NAME, dp->con->err)
            )

            OCI_ReleaseMetaString(ostr);
        }

        /* set array size attribute */

         OCI_CALL2
        (
            res, dp->con,

            OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                       (dvoid *) &dp->nb_rows, (ub4) sizeof(ub4),
                       (ub4) OCI_ATTR_NUM_ROWS, dp->con->err)
        )

        /* set columns count attribute */

        OCI_CALL2
        (
            res, dp->con,

            OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                       (dvoid *) &dp->nb_cols, (ub4) sizeof(ub4),
                       (ub4) OCI_ATTR_NUM_COLS, dp->con->err)
        )

        /* allocating the column array */

        if (res == TRUE)
        {
            dp->cols = (void *) OCI_MemAlloc(OCI_IPC_DP_COL_ARRAY,
                                             sizeof(OCI_DirPathColumn),
                                             dp->nb_cols, TRUE);

            res = (dp->cols != NULL);
        }
    }
    else
        res = FALSE;

   /* handle errors */

    if (res == FALSE)
    {
        OCI_DirPathFree(dp);
        dp = NULL;
    }

    OCI_RESULT(res);

    return dp;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathFree(OCI_DirPath *dp)
{
    ub4 i;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    /* check state for doing any closure ???? */


    for (i = 0; i < dp->nb_cols; i++)
    {
        OCI_FREE(dp->cols[i].name);
        OCI_FREE(dp->cols[i].format);
    }

    OCI_FREE(dp->cols);

    OCI_FREE(dp->schema);
    OCI_FREE(dp->table);
    OCI_FREE(dp->partition);

    OCI_HandleFree(dp->strm, OCI_HTYPE_DIRPATH_STREAM);
    OCI_HandleFree(dp->arr,  OCI_HTYPE_DIRPATH_COLUMN_ARRAY);
    OCI_HandleFree(dp->ctx,  OCI_HTYPE_DIRPATH_CTX);

    OCI_FREE(dp);

    OCI_RESULT(TRUE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathSetColumn
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathSetColumn(OCI_DirPath *dp, unsigned int index,
                                     mtext *name, unsigned int size,
                                     mtext *format, int prec, int scale,
                                     boolean binary)
{
    OCI_DirPathColumn *col = NULL;
    OCIParam *hattr = NULL;
    OCIParam *hlist = NULL;
    void *ostr  = NULL;
    int osize   = -1;
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_ALLOCATED, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, FALSE);
    OCI_CHECK_BOUND(dp->con, index, 1, dp->nb_cols, FALSE);

    col = &dp->cols[index-1];

    /* reset fields */

    if (col->name != NULL)
        OCI_FREE(col->name);

    if (col->format != NULL)
        OCI_FREE(col->format);

    /* setup fields */

    col->name    = mtsdup(name);
    col->format  = mtsdup(format);
    col->prec    = (sb2) prec;
    col->scale   = (sb1) scale;
    col->size    = (ub2) size;
    col->type    = (ub1) (binary ? SQLT_BIN : SQLT_CHR);

    if (binary == FALSE)
        col->size *= sizeof(dtext);

    /* get column parameter list handle */

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrGet(dp->ctx, OCI_HTYPE_DIRPATH_CTX, &hlist,
                   NULL, OCI_ATTR_LIST_COLUMNS, dp->con->err)
    )

   /* get colum attribute handle */

    OCI_CALL2
    (
        res, dp->con,

        OCIParamGet((dvoid *) hlist, OCI_DTYPE_PARAM, dp->con->err,
                    (dvoid** ) &hattr, (ub4) index)
    )

    /* set column name */

    if (res == TRUE)
    {
        osize = -1;
        ostr  = OCI_GetInputMetaString(col->name, &osize);

        OCI_CALL2
        (
            res, dp->con,

            OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) ostr, (ub4) osize,
                       (ub4) OCI_ATTR_NAME, dp->con->err)
        )

        OCI_ReleaseMetaString(ostr);
    }

    /* set column type */

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                   (dvoid *) &col->type, sizeof(col->type),
                   (ub4) OCI_ATTR_DATA_TYPE, dp->con->err)
    )

    /* set column size */

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                   (dvoid *) &col->size, sizeof(col->size),
                   (ub4) OCI_ATTR_DATA_SIZE, dp->con->err)
    )

    /* set column precision */

    if (col->prec > 0)
    {
        OCI_CALL2
        (
            res, dp->con,

            OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) &col->prec, sizeof(col->prec),
                       (ub4) OCI_ATTR_PRECISION, dp->con->err)
        )
    }

    /* set column scale */

    if (col->scale > 0)
    {
        OCI_CALL2
        (
            res, dp->con,

            OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) &col->scale, sizeof(col->scale),
                       (ub4) OCI_ATTR_SCALE, dp->con->err)
        )
    }

    /* set column date/time format attribute */

    if ((res == TRUE) && (col->format != NULL) && (col->format[0] != 0))
    {
        osize = -1;
        ostr  = OCI_GetInputMetaString(col->format, &osize);

        OCI_CALL2
        (
            res, dp->con,

            OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) ostr, (ub4) osize,
                       (ub4) OCI_ATTR_DATEFORMAT, dp->con->err)
        )

        OCI_ReleaseMetaString(ostr);
    }

#ifndef OCI_CHARSET_ANSI

    /* setup Unicode mode for unicode user data  */

   if (binary == FALSE)
   {
        ub2 csid = OCI_UTF16ID;

        OCI_CALL2
        (
            res, dp->con,

            OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) &csid,  (ub4) sizeof(csid),
                       (ub4) OCI_ATTR_CHARSET_ID,  dp->con->err)
        )
    }

#endif     

    /* free param handle */

    res = (OCI_SUCCESS == OCIDescriptorFree(hattr, OCI_DTYPE_PARAM));

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathPrepare
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathPrepare(OCI_DirPath *dp)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_ALLOCATED, FALSE);

    /* prepare direct path operation */

    OCI_CALL2
    (
        res, dp->con,

        OCIDirPathPrepare(dp->ctx, dp->con->cxt, dp->con->err)
    )

    /* allocate column array handle */

    if (res == TRUE)
    {
        res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) dp->ctx,
                                              (dvoid **) (void *) &dp->arr,
                                              (ub4) OCI_HTYPE_DIRPATH_COLUMN_ARRAY,
                                              (size_t) 0, (dvoid **) NULL));
    }

    /* allocate stream handle */

    if (res == TRUE)
    {
       res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) dp->ctx,
                                              (dvoid **) (void *) &dp->strm,
                                              (ub4) OCI_HTYPE_DIRPATH_STREAM,
                                              (size_t) 0, (dvoid **) NULL));
    }

    /* check the number of rows allocated */

    if (res == TRUE)
    {
        ub4 size = sizeof(dp->nb_rows);

        OCI_CALL2
        (
            res, dp->con,

            OCIAttrGet(dp->arr, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &dp->nb_rows,
                       &size, OCI_ATTR_NUM_ROWS, dp->con->err)
        )

        dp->nb_cur = dp->nb_rows;
    }

    if (res == TRUE)
        dp->status = OCI_DPS_PREPARED;

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathSetEntry
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathSetEntry(OCI_DirPath *dp, unsigned int row,
                                    unsigned int index, void *value,
                                    int size,  boolean complete)
{
    boolean res = TRUE;
    OCI_DirPathColumn *col = NULL;

    ub4 usize;
    ub1 flag;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, FALSE);
    OCI_CHECK_BOUND(dp->con, index, 1, dp->nb_cols, FALSE);
    OCI_CHECK_BOUND(dp->con, row, 1, dp->nb_cur, FALSE);

    col = &dp->cols[index-1];

    OCI_CHECK_BOUND(dp->con, size, -1, (int) col->size, FALSE);

    /* setup column flag */

    if ((value == NULL) || (size == 0))
    {
        flag = OCI_DIRPATH_COL_NULL;
    }
    else if (complete == TRUE)
    {
        flag = OCI_DIRPATH_COL_COMPLETE;
    }
    else
    {
        flag = OCI_DIRPATH_COL_PARTIAL;
    }

    /* check size */

    if (size < 0)
    {
        if (col->type == SQLT_CHR)
            usize = (ub4) dtslen((dtext *) value);
        else
            usize = col->size;
    }
    else
        usize = size;

    /* for character based column, parameter size was the number of character */

    if (col->type == SQLT_CHR)
    {
       usize *= sizeof(dtext);
    }

    /* set entry value */

	OCI_CALL2
    (
        res, dp->con,

        OCIDirPathColArrayEntrySet(dp->arr, dp->con->err, (ub4) (row-1),
                                   (ub2) (index-1), (ub1*) value,
                                   (ub4) usize, flag)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathSetArray
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathSetArray(OCI_DirPath *dp, unsigned int index,
                                    void *values, int *sizes)
{
    boolean res = TRUE;
    ub1 *value  = NULL;

    int size;
    ub4 usize;
    ub4 i;

    ub1 flag;

    OCI_DirPathColumn *col = NULL;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, FALSE);

    OCI_CHECK_PTR(OCI_IPC_VOID, values, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, sizes, FALSE);

    OCI_CHECK_BOUND(dp->con, index, 1, dp->nb_cols, FALSE);

    col = &dp->cols[index-1];

    for (i = 0; (res == TRUE) && (i < dp->nb_cur); i++)
    {
        /* get values from arrays */

        value = (ub1 *) values;
        size  = sizes[i];

        value += (i * col->size);

        /* setup column flag */

        if ((value == NULL) || (size == 0))
        {
            flag = OCI_DIRPATH_COL_NULL;
        }
        else if (complete == TRUE)
        {
            flag = OCI_DIRPATH_COL_COMPLETE;
        }
        else
        {
            flag = OCI_DIRPATH_COL_PARTIAL;
        }

        /* check size */

        if (size < 0)
        {
            if (col->type == SQLT_CHR)
                usize = (ub4) dtslen((dtext *) value);
            else
                usize = col->size;
        }
        else
            usize = size;

        /* for character based column, parameter size was the number of character */

        if (col->type == SQLT_CHR)
        {
           usize *= sizeof(dtext);
        }



        if (sizes[i] < 0)
        {
            if (col->type == SQLT_CHR)
                usize = (ub4) (dtslen((dtext *) value));
            else
                usize = col->size;
        }
        else if (sizes[i] == 0)
        {
            usize = 
        }
        else
            size = (ub4) sizes[i];

        if (col->type == SQLT_CHR)
        {
            size  = (size)*sizeof(dtext);
        }

	    OCI_CALL2
        (
            res, dp->con,

            OCIDirPathColArrayEntrySet(dp->arr, dp->con->err, (ub4) i,
                                       (ub2) (index-1), (ub1 *) value,
                                       (ub4) size, (ub1) OCI_DIRPATH_COL_COMPLETE)
        )
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathReset
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathReset(OCI_DirPath *dp)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

   /* reset array */

    OCI_CALL2
    (
        res, dp->con,

        OCIDirPathColArrayReset(dp->arr, dp->con->err)
    )

   /* reset stream */

    OCI_CALL2
    (
        res, dp->con,

        OCIDirPathStreamReset(dp->strm, dp->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathConvert
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_DirPathConvert(OCI_DirPath *dp)
{
    unsigned int res = OCI_DPR_COMPLETE;
    sword ret = OCI_SUCCESS;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, FALSE);

    dp->err_col  = -1;
    dp->nb_prcsd =  0;

    ret = OCIDirPathColArrayToStream(dp->arr, dp->ctx,  dp->strm, dp->con->err, 
                                     (ub4) dp->nb_cur, (ub4) 0);

    switch (ret)
    {
        case OCI_SUCCESS:
        {
            dp->status    = OCI_DPS_CONVERTED;
            dp->nb_prcsd  = dp->nb_cur;
            dp->err_col   = -1;
            res           = OCI_DPR_COMPLETE;

            break;
        }
        case OCI_ERROR:
        {
            res = OCI_DPR_ERROR;

            OCI_ExceptionOCI(dp->con->err, dp->con, NULL);

            break;
       }
       case OCI_CONTINUE:
        {
            res = OCI_DPR_FULL;

            break;
        }
       case OCI_NEED_DATA:
        {
            res = OCI_DPR_PARTIAL;

            break;
        }

    }

    if (ret != OCI_SUCCESS)
    {
        ub4 size;

        size = sizeof(dp->nb_prcsd);

        OCIAttrGet(dp->arr, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &dp->nb_prcsd,
                   &size, OCI_ATTR_NUM_ROWS, dp->con->err);

        size = sizeof(dp->err_col);

        OCIAttrGet(dp->arr, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &dp->err_col,
                   &size, OCI_ATTR_COL_COUNT, dp->con->err);

        OCI_ATTR_ROW_COUNT

    }

    OCI_RESULT(ret == OCI_SUCCESS);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathLoad
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_DirPathLoad(OCI_DirPath *dp)
{
    unsigned int res = OCI_DPR_COMPLETE;
    sword ret = OCI_SUCCESS;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_CONVERTED, FALSE);

    dp->nb_prcsd =  0;
    dp->err_col  =  -1;

    ret = OCIDirPathLoadStream(dp->ctx, dp->strm, dp->con->err);

    switch (ret)
    {
        case OCI_SUCCESS:
        {
            dp->status     = OCI_DPS_PREPARED;
            dp->nb_prcsd   = dp->nb_cur;
            dp->nb_loaded += dp->nb_cur;
            res            = OCI_DPR_COMPLETE;

            break;
        }
        case OCI_ERROR:
        {
            res = OCI_DPR_ERROR;

            OCI_ExceptionOCI(dp->con->err, dp->con, NULL);

            break;
       }
       case OCI_NO_DATA:
        {
            res = OCI_DPR_EMPTY;

            break;
        }
       case OCI_NEED_DATA:
        {
            res = OCI_DPR_PARTIAL;

            break;
        }

    }

    if (ret != OCI_SUCCESS)
    {
        ub4 size = sizeof(dp->nb_prcsd);

         OCIAttrGet(dp->arr, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &dp->nb_prcsd,
                    &size, OCI_ATTR_NUM_ROWS, dp->con->err);
    }

    OCI_RESULT(ret == OCI_SUCCESS);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathFinish
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathFinish(OCI_DirPath *dp)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, FALSE);

    OCI_DirPathReset(dp);

	OCI_CALL2
    (
        res, dp->con,

        OCIDirPathFinish(dp->ctx, dp->con->err)
    )

    if (res == TRUE)
        dp->status = OCI_DPS_FINISHED;

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathAbort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathAbort(OCI_DirPath *dp)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, FALSE);

	OCI_CALL2
    (
        res, dp->con,

        OCIDirPathAbort(dp->ctx, dp->con->err)
    )

    if (res == TRUE)
        dp->status = OCI_DPS_ALLOCATED;

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathSave
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathSave(OCI_DirPath *dp)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, FALSE);

    OCI_DirPathReset(dp);

	OCI_CALL2
    (
        res, dp->con,

        OCIDirPathDataSave(dp->ctx, dp->con->err, OCI_DIRPATH_DATASAVE_SAVEONLY)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathFlushRow
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathFlushRow(OCI_DirPath *dp)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, FALSE);

	OCI_CALL2
    (
        res, dp->con,

        OCIDirPathFlushRow(dp->ctx, dp->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathSetSize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathSetSize(OCI_DirPath *dp, unsigned int nb_rows)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, FALSE);

    OCI_CHECK_BOUND(dp->con, nb_rows, 1, dp->nb_rows, FALSE);

    dp->nb_cur = nb_rows;

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathSetDateFormat
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathSetDateFormat(OCI_DirPath *dp, mtext *format)
{
    boolean res = TRUE;
    void *ostr  = NULL;
    int osize   = -1;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    osize = -1;
    ostr  = OCI_GetInputMetaString(format, &osize);

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) ostr, (ub4) osize,
                   (ub4) OCI_ATTR_DATEFORMAT, dp->con->err)
    )

    OCI_ReleaseMetaString(ostr);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathSetParallel
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathSetParallel(OCI_DirPath *dp, boolean value)
{
    boolean res = TRUE;
    ub1 enabled = (ub1) value;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &enabled, (ub4) sizeof(enabled),
                   (ub4) OCI_ATTR_DIRPATH_PARALLEL, dp->con->err)
    )

    OCI_RESULT(res);

    return res;}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathSetNoLog
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathSetNoLog(OCI_DirPath *dp, boolean value)
{
    boolean res = TRUE;
    ub1 nolog   = (ub1) value;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &nolog, (ub4) sizeof(nolog),
                   (ub4) OCI_ATTR_DATEFORMAT, dp->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathEnableCache
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathEnableCache(OCI_DirPath *dp, boolean value)
{
    boolean res = TRUE;
    ub1 enabled = (ub1) value;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &enabled, (ub4) sizeof(enabled),
                   (ub4) OCI_ATTR_DIRPATH_DCACHE_DISABLE, dp->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathSetCacheSize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathSetCacheSize(OCI_DirPath *dp, unsigned int size)
{
    boolean res     = TRUE;
    ub4 cache_size  = size;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &cache_size, (ub4) sizeof(cache_size),
                   (ub4) OCI_ATTR_DIRPATH_DCACHE_SIZE, dp->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathSetBufferSize
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_DirPathSetBufferSize(OCI_DirPath *dp, unsigned int size)
{
    boolean res  = TRUE;
    ub4 bufsize  = (ub4)size;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &bufsize, (ub4) sizeof(bufsize),
                   (ub4) OCI_ATTR_BUF_SIZE, dp->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathGetLoadedCount
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_DirPathGetLoadedCount(OCI_DirPath *dp)
{
    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_RESULT(TRUE);

    return dp->nb_loaded;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathGetProcessedCount
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_DirPathGetProcessedCount(OCI_DirPath *dp)
{
    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_RESULT(TRUE);

    return dp->nb_rows;
}

/* ------------------------------------------------------------------------ *
 * OCI_DirPathGetErrorColumn
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_DirPathGetErrorColumn(OCI_DirPath *dp)
{
    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_RESULT(TRUE);

    return dp->err_col + 1;
}
