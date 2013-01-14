/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2012 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: dirpath.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"


/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */


/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetArray
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_DirPathSetArray
(
    OCI_DirPath *dp,
    ub4 row_from
)
{
    boolean  res     = TRUE;
    ub1     *data    = NULL;
    ub4      size    = 0;
    ub1      flag    = 0;
    ub2      col     = 0;
    ub4      row     = 0;

    /* reset the number of entries et */

    dp->nb_entries = 0;

    /* set entries */

    for (row = row_from; (row < dp->nb_cur) && (res == TRUE); row++)
    { 
        for (col = 0; (col < dp->nb_cols) && (res == TRUE); col++)
        {
            OCI_DirPathColumn *dpcol = &(dp->cols[col]); 

            /* get internal data cell */

            data = ((ub1 *) dpcol->data) +  (size_t) (row * dpcol->bufsize);
            size = dpcol->lens[row];
            flag = dpcol->flags[row];
                 
            if (dpcol->sqlcode == SQLT_NUM)
            {
                OCINumber *num = (OCINumber *) data;

                data = &num->OCINumberPart[1];
            }

            /* set entry value */

            OCI_CALL2
            (
                res, dp->con,

                OCIDirPathColArrayEntrySet(dp->arr, dp->con->err, (ub4) dp->nb_entries,
                                            (ub2) (col), (ub1*) data, (ub4) size, flag)
            )
        }

        /* increment number of item set */

        if (res == TRUE)
        {
            dp->nb_entries++;
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPahArrayToStream
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPahArrayToStream
(
    OCI_DirPath *dp,
    ub4 row_from
)
{
    unsigned int res  = OCI_DPR_COMPLETE;
    sword        ret  = OCI_SUCCESS;
    
    /* convert the array to a stream */

    ret = OCIDirPathColArrayToStream(dp->arr, dp->ctx, dp->strm, dp->con->err, dp->nb_entries, (ub4) 0);

    switch (ret)
    {
        case OCI_SUCCESS:
        {
            res        = OCI_DPR_COMPLETE;
            dp->status = OCI_DPS_CONVERTED;

            break;
        }
        case OCI_ERROR:
        {
            res = OCI_DPR_ERROR;

            /* only raise the exception if we're not in force mode */

            if (dp->cvt_mode == OCI_DCM_DEFAULT)
            {
                OCI_ExceptionOCI(dp->con->err, dp->con, NULL, FALSE);
            }

            break;
        }
        case OCI_CONTINUE:
        {
            dp->status = OCI_DPS_CONVERTED;
            res        = OCI_DPR_FULL;

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
        ub4 err_row = 0;
        ub2 err_col = 0;
        ub4 size    = 0;
 
        size = sizeof(err_col);

        OCIAttrGet(dp->arr, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &err_col,
                    &size, OCI_ATTR_COL_COUNT, dp->con->err);

        size = sizeof(err_row);

        OCIAttrGet(dp->arr, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &err_row,
                    &size, OCI_ATTR_ROW_COUNT, dp->con->err);

        /* update converted rows so far */
        dp->nb_converted += err_row;

        /* record errors index */
        dp->err_rows[dp->nb_err] = row_from + err_row;
        dp->err_cols[dp->nb_err] = err_col;

        dp->nb_err++;
    }
    else
    {
        /* conversion is successful. the number of converted rows is the same 
           as the number of row set*/
        dp->nb_converted += dp->nb_entries;
    }

    return res;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathCreate
 * --------------------------------------------------------------------------------------------- */

OCI_DirPath * OCI_API OCI_DirPathCreate
(
    OCI_TypeInfo *typinf,
    const mtext  *partition,
    unsigned int  nb_cols,
    unsigned int  nb_rows
)
{
    OCI_DirPath *dp  = NULL;
    boolean      res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);

    OCI_CHECK_COMPAT(typinf->con, typinf->type != OCI_TIF_TYPE, NULL);
    OCI_CHECK_BOUND(typinf->con, nb_cols, 1, typinf->nb_cols, NULL);

    /* allocate direct path structure */

    dp = (OCI_DirPath *) OCI_MemAlloc(OCI_IPC_DIRPATH, sizeof(*dp), (size_t) 1, TRUE);

    if (dp != NULL)
    {
        void *ostr = NULL;
        int osize  = -1;
    
        dp->con          = typinf->con;
        dp->status       = OCI_DPS_NOT_PREPARED;
        dp->typinf       = typinf;
        dp->nb_rows      = (ub2) nb_rows;
        dp->nb_cols      = (ub2) nb_cols;
        dp->nb_cur       = (ub2) dp->nb_rows;
        dp->nb_entries   = 0;
        dp->nb_err       = 0;
        dp->idx_err_col  = 0;
        dp->idx_err_row  = 0;
        dp->nb_converted = 0;
        dp->nb_loaded    = 0;
        dp->nb_converted = 0;
        dp->cvt_mode     = OCI_DCM_DEFAULT;

        /* allocates direct context handle */

        if (res == TRUE)
        {
            res = (OCI_SUCCESS == OCI_HandleAlloc((dvoid *) dp->con->env,
                                                  (dvoid **) (void *) &dp->ctx,
                                                  (ub4) OCI_HTYPE_DIRPATH_CTX,
                                                  (size_t) 0, (dvoid **) NULL));
        }

        /* set table name attribute */

        if (res == TRUE)
        {
            osize = -1;
            ostr  = OCI_GetInputMetaString(dp->typinf->name, &osize);

            OCI_CALL2
            (
                res, dp->con,

                OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                           (dvoid *) ostr, (ub4) osize, (ub4) OCI_ATTR_NAME, dp->con->err)
            )

            OCI_ReleaseMetaString(ostr);
        }

        /* set schema name attribute */

        if ((res == TRUE) && (dp->typinf->schema != NULL) && (dp->typinf->schema[0] != 0))
        {
            osize = -1;
            ostr  = OCI_GetInputMetaString(dp->typinf->schema, &osize);

            OCI_CALL2
            (
                res, dp->con,

                OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                           (dvoid *) ostr, (ub4) osize, (ub4) OCI_ATTR_SCHEMA_NAME, dp->con->err)
            )

            OCI_ReleaseMetaString(ostr);
        }

        /* set partition name attribute */

        if ((res == TRUE) && (partition != NULL) && (partition[0] != 0))
        {
            osize = -1;
            ostr  = OCI_GetInputMetaString(partition, &osize);

            OCI_CALL2
            (
                res, dp->con,

                OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                           (dvoid *) ostr, (ub4) osize, (ub4) OCI_ATTR_SUB_NAME, dp->con->err)
            )

            OCI_ReleaseMetaString(ostr);
        }

        if (OCILib.version_runtime >= OCI_9_0)
        {
            ub4 num_rows = dp->nb_rows;

            /* set array size attribute */

            OCI_CALL2
            (
                res, dp->con,

                OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                           (dvoid *) &num_rows, (ub4) sizeof(num_rows),
                           (ub4) OCI_ATTR_NUM_ROWS, dp->con->err)
            )
        }

        /* set columns count attribute */

        OCI_CALL2
        (
            res, dp->con,

            OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                       (dvoid *) &dp->nb_cols, (ub4) sizeof(dp->nb_cols),
                       (ub4) OCI_ATTR_NUM_COLS, dp->con->err)
        )

        /* allocating the column array */

        if (res == TRUE)
        {
            dp->cols = (void *) OCI_MemAlloc(OCI_IPC_DP_COL_ARRAY, sizeof(OCI_DirPathColumn),
                                             (size_t) dp->nb_cols, TRUE);

            res = (dp->cols != NULL);
        }
    }
    else
    {
        res = FALSE;
    }

    /* handle errors */

    if (res == FALSE)
    {
        OCI_DirPathFree(dp);
        dp = NULL;
    }

    OCI_RESULT(res);

    return dp;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathFree
(
    OCI_DirPath *dp
)
{
    ub2 i;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    for (i = 0; i < dp->nb_cols; i++)
    {
        OCI_FREE(dp->cols[i].data);
        OCI_FREE(dp->cols[i].lens);
        OCI_FREE(dp->cols[i].flags);
        OCI_FREE(dp->cols[i].format);
    }

    OCI_FREE(dp->cols);
    OCI_FREE(dp->err_cols);
    OCI_FREE(dp->err_rows);

    OCI_HandleFree(dp->strm, OCI_HTYPE_DIRPATH_STREAM);
    OCI_HandleFree(dp->arr,  OCI_HTYPE_DIRPATH_COLUMN_ARRAY);
    OCI_HandleFree(dp->ctx,  OCI_HTYPE_DIRPATH_CTX);

    OCI_FREE(dp);

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetColumn
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSetColumn
(
    OCI_DirPath *dp,
    unsigned int index,
    const mtext *name,
    unsigned int maxsize,
    const mtext *format
)
{
    OCI_DirPathColumn *dpcol = NULL;
    OCI_Column *col          = NULL;
    OCIParam *hattr          = NULL;
    OCIParam *hlist          = NULL;
    void *ostr               = NULL;
    int osize                = -1;
    boolean res              = TRUE;
    ub2 i;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, FALSE);
    OCI_CHECK_BOUND(dp->con, index, 1, dp->nb_cols, FALSE);

    /* check if column exists */

    for (i = 0; i < dp->typinf->nb_cols; i++)
    {
        if (mtscasecmp(name, dp->typinf->cols[i].name) == 0)
        {
            break;
        }
    }

    /* check if column was found */

    if (i >= dp->typinf->nb_cols)
    {
        OCI_ExceptionDirPathColNotFound(dp, name, dp->typinf->name);

        res = FALSE;
    }

    /* set column information */

    if (res == TRUE)
    {
        col   = &dp->typinf->cols[i];
        dpcol = &dp->cols[index-1];

        /* default column attributes */

        dpcol->maxsize     = (ub2) maxsize;
        dpcol->bufsize     = (ub2) maxsize + 1;
        dpcol->sqlcode     = SQLT_CHR;
        dpcol->type        = OCI_DDT_TEXT;
        dpcol->index       = i;
        dpcol->format_size = 0;

        switch (col->type)
        {
            case OCI_CDT_TEXT:
            {
                dpcol->maxsize *= sizeof(dtext);
                dpcol->bufsize *= sizeof(dtext);

                if (OCILib.nls_utf8 == TRUE)
                {
                    dpcol->bufsize *= UTF8_BYTES_PER_CHAR;
                }

                break;
            }
            case OCI_CDT_NUMERIC:
            {
                if ((format != NULL) && (format[0] != 0))
                {
                    dpcol->format      = mtsdup(format);
                    dpcol->format_size = (ub4) mtslen(format);
                    dpcol->type        = OCI_DDT_NUMBER;
                    dpcol->sqlcode     = SQLT_NUM;
                    dpcol->bufsize     = sizeof(OCINumber);
                    dpcol->maxsize     = sizeof(OCINumber);
                }
                else
                {
                    dpcol->type = OCI_DDT_OTHERS;
                }

                break;
            }
            case OCI_CDT_DATETIME:
            case OCI_CDT_TIMESTAMP:
            case OCI_CDT_INTERVAL:
            {
                dpcol->type = OCI_DDT_OTHERS;

                if ((format != NULL) && (format[0] != 0))
                {
                    dpcol->format      = mtsdup(format);
                    dpcol->format_size = (ub4) mtslen(format);
                    dpcol->maxsize     = (ub2) dpcol->format_size;
                    dpcol->bufsize    *= sizeof(dtext);
                }

                break;
            }
            case OCI_CDT_LOB:
            {
                if (col->subtype == OCI_BLOB)
                {
                    dpcol->type    = OCI_DDT_BINARY;
                    dpcol->sqlcode = SQLT_BIN;
                }

                break;
            }
            case OCI_CDT_LONG:
            {
                if (col->subtype == OCI_BLONG)
                {
                    dpcol->type    = OCI_DDT_BINARY;
                    dpcol->sqlcode = SQLT_BIN;
                }

                break;
            }
            case OCI_CDT_RAW:
            {
                dpcol->type    = OCI_DDT_BINARY;
                dpcol->sqlcode = SQLT_BIN;

                break;
            }
            default:
            {
                res = FALSE;
                OCI_ExceptionDatatypeNotSupported(dp->con, NULL, col->ocode);

                break;
            }
        }
    }

    /* if supported datatype, set direct path column attributes */

    if (res == TRUE)
    {
        /* get column parameter list handle */

        OCI_CALL2
        (
            res, dp->con,

            OCIAttrGet(dp->ctx, OCI_HTYPE_DIRPATH_CTX, &hlist, NULL, OCI_ATTR_LIST_COLUMNS, dp->con->err)
        )

        /* get colum attribute handle */

        OCI_CALL2
        (
            res, dp->con,

            OCIParamGet((dvoid *) hlist, OCI_DTYPE_PARAM, dp->con->err,
                        (dvoid** ) (dvoid *) &hattr, (ub4) index)
        )

        /* set column name */

        if (res == TRUE)
        {
            osize = -1;
            ostr  = OCI_GetInputMetaString(name, &osize);

            OCI_CALL2
            (
                res, dp->con,

                OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                           (dvoid *) ostr, (ub4) osize, (ub4) OCI_ATTR_NAME, dp->con->err)
            )

            OCI_ReleaseMetaString(ostr);
        }

        /* set column type */

        OCI_CALL2
        (
            res, dp->con,

            OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) &dpcol->sqlcode, sizeof(dpcol->sqlcode),
                       (ub4) OCI_ATTR_DATA_TYPE, dp->con->err)
        )

        /* set column size */

        OCI_CALL2
        (
            res, dp->con,

            OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) &dpcol->maxsize, sizeof(dpcol->maxsize),
                       (ub4) OCI_ATTR_DATA_SIZE, dp->con->err)
        )

        /* set column precision */

        if (col->prec != 0)
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

        if (col->scale != 0)
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

        if ((res == TRUE) && (dpcol->type != OCI_DDT_NUMBER) &&
            (dpcol->format != NULL) && (dpcol->format[0] != 0))
        {
            osize = -1;
            ostr  = OCI_GetInputMetaString(dpcol->format, &osize);

            OCI_CALL2
            (
                res, dp->con,

                OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                           (dvoid *) ostr, (ub4) osize,
                           (ub4) OCI_ATTR_DATEFORMAT, dp->con->err)
            )

            OCI_ReleaseMetaString(ostr);
        }

    #ifdef OCI_CHARSET_WIDE

        /* setup Unicode mode for Unicode user data */

        if (dpcol->type == OCI_DDT_TEXT)
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

        OCIDescriptorFree(hattr, OCI_DTYPE_PARAM);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathPrepare
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathPrepare
(
    OCI_DirPath *dp
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED, FALSE);

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
        ub4 num_rows = 0;
        ub4 size     = sizeof(num_rows);

        OCI_CALL2
        (
            res, dp->con,

            OCIAttrGet(dp->arr, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &num_rows,
                       &size, OCI_ATTR_NUM_ROWS, dp->con->err)
        )

        dp->nb_cur  = (ub2) num_rows;
        dp->nb_rows = (ub2) num_rows;
    }

    /* allocate array of errs rows */

    if (res == TRUE)
    {
        dp->err_rows = (ub4 *) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, sizeof(*dp->err_rows),
                                            (size_t) dp->nb_cur, TRUE);

        res = (dp->err_rows != NULL);
    }

    /* allocate array of errs cols */

    if (res == TRUE)
    {
        dp->err_cols = (ub2 *) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, sizeof(*dp->err_cols),
                                            (size_t) dp->nb_cur, TRUE);

        res = (dp->err_cols != NULL);
    }
    
    /* now, we need to allocate internal buffers */

    if (res == TRUE)
    {
        ub2 i;

        for (i = 0; i < dp->nb_cols; i++)
        {
            OCI_DirPathColumn *col = &dp->cols[i];

            /* data buffers */

            col->data = (ub1 *) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY,
                                             (size_t) col->bufsize,
                                             (size_t) dp->nb_cur, TRUE);

            if (col->data == NULL)
            {
                res = FALSE;
                break;
            }

            /* data sizes */

            col->lens = (ub4 *) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, sizeof(ub4),
                                             (size_t) dp->nb_cur, TRUE);

            if (col->lens == NULL)
            {
                res = FALSE;
                break;
            }

            /* data flags */

            col->flags = (ub1 *) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, sizeof(ub1),
                                              (size_t) dp->nb_cur, TRUE);

            if (col->flags == NULL)
            {
                res = FALSE;
                break;
            }
        }
    }

    if (res == TRUE)
    {
        dp->status = OCI_DPS_PREPARED;
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetEntry
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSetEntry
(
    OCI_DirPath *dp,
    unsigned int row,
    unsigned int index,
    void        *value,
    unsigned int size,
    boolean      complete
)
{
    boolean res              = TRUE;
    OCI_DirPathColumn *dpcol = NULL;

    ub1 *data;
    ub1 flag;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, FALSE);
    OCI_CHECK_BOUND(dp->con, index, 1, dp->nb_cols, FALSE);
    OCI_CHECK_BOUND(dp->con, row, 1, dp->nb_cur, FALSE);
 
    dpcol = &dp->cols[index-1];

    if (dpcol != NULL)
    {
        /* check size */

        if (size > dpcol->maxsize)
        {
            size = (unsigned int) dpcol->maxsize;
        }

        /* setup column flag */

        if (value == NULL)
        {
            flag = OCI_DIRPATH_COL_NULL;
            size = 0;
        }
        else if (complete == TRUE)
        {
            flag = OCI_DIRPATH_COL_COMPLETE;
        }
        else
        {
            flag = OCI_DIRPATH_COL_PARTIAL;
        }

        /* Process only if data is not null */
        
        if (value != NULL)
        {
            /* for character based column, parameter size was the number of characters */

            if (dpcol->sqlcode == SQLT_CHR)
            {
                size *= (unsigned int) sizeof(dtext);
            }

            /* get internal data cell */

            data = ((ub1 *) dpcol->data) + (size_t) ((row-1) * dpcol->bufsize);

        #if defined(OCI_CHECK_DATASTRINGS)

            /* we weed to pack the buffer if wchar_t is 4 bytes */

            if (dpcol->type == OCI_DDT_TEXT)
            {
                int osize = -1;

                OCI_GetOutputString(value, data, &size, sizeof(dtext), sizeof(odtext));
            }
            else

        #endif

        #if defined(OCI_USERDATA_WIDE)

            /* input Unicode numeric values causes oracle conversion error.
               so, let's convert them to ansi */

            if (dpcol->type == OCI_DDT_OTHERS)
            {
                size = (unsigned int) wcstombs((char *) data, value, dpcol->bufsize - 1);
            }
            else

        #endif

            /* if a format was provided for a numeric column, we convert the input
               buffer to a OCINumber */

            if (dpcol->type == OCI_DDT_NUMBER)
            {
                OCINumber *num = (OCINumber *) data;

                res = OCI_NumberFromString(dp->con, num, sizeof(*num), OCI_NUM_NUMBER, SQLT_NUM, (dtext *) value, dpcol->format);
 
                if (res == TRUE)
                {
                    size = (unsigned int) num->OCINumberPart[0];
                }
            }
            else
            {

            #if defined(OCI_CHARSET_MIXED)

                /* with mixed charset builds, OCIDirPrepare() causes a segfault if the
                   attribute OCI_ATTR_CHARSET_ID has been set with OCI_UTF16.
                   In the OCILIB direct path implementation, the code is the same for
                   Unicode and mixed charset. This only difference is the
                   environment mode set of UTF16...
                   So let's convert the data back to ANSI until Oracle corrects this bug */

                if (dpcol->type == OCI_DDT_TEXT)
                {
                    size = (unsigned int) wcstombs((char *) data, value, dpcol->bufsize - 1);
                }
                else

            #endif

                {
                    memcpy(data, value, (size_t) size);
                }
            }
        }

        dpcol->lens[row-1]  = size;
        dpcol->flags[row-1] = flag;
    }
    else
    {
        res = FALSE;
    }
    
    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathReset
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathReset
(
    OCI_DirPath *dp
)
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

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathConvert
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathConvert
(
    OCI_DirPath *dp
)
{
    unsigned int ret      = OCI_DPR_ERROR;
    ub4          row_from = 0;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, OCI_DPR_ERROR);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, OCI_DPR_ERROR);

    dp->nb_processed = 0;

    /* perform full variable reset in forcemode or if we did not encounter errors in default mode*/

    if ((dp->cvt_mode == OCI_DCM_FORCE) || (dp->cvt_mode == OCI_DCM_DEFAULT && dp->nb_err == 0))
    {
        dp->nb_converted    = 0;
        dp->nb_err          = 0;
        dp->idx_err_row     = 0;
        dp->idx_err_col     = 0;
    }

    /* in case of previous error in default mode, start again from the last faulted row */

    if ((dp->cvt_mode == OCI_DCM_DEFAULT) && (dp->nb_err > 0))
    {
        row_from = dp->err_rows[dp->nb_err - 1];
    }

    /* set array values */

    if (OCI_DirPathSetArray(dp, row_from) == TRUE)
    {
        /* try to convert values from array into stream */

        ret = OCI_DirPahArrayToStream(dp, row_from);

        /* in case of conversion error, continue conversion in force mode
           other return from conversion */

        if (dp->cvt_mode == OCI_DCM_FORCE && ret == OCI_DPR_ERROR)
        {
            boolean res = TRUE;

            /* perfom conversion until all non erred rows are converted */

            while ((ret == OCI_DPR_ERROR) && (dp->nb_err <= dp->nb_cur) && (res == TRUE))
            {
                /* start from the row that follows the last erred row */

                row_from = dp->err_rows[dp->nb_err - 1] + 1;

                /* set values again */

                res = OCI_DirPathSetArray(dp, row_from);

                if (res == TRUE)
                {
                     /* perform conversion again */

                     ret = OCI_DirPahArrayToStream(dp, row_from);
                }
            }
        }
    }

    dp->nb_processed = dp->nb_converted;

    OCI_RESULT(ret ==  OCI_DPR_COMPLETE);

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathLoad
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathLoad
(
    OCI_DirPath *dp
)
{
    unsigned int res = OCI_DPR_COMPLETE;
    sword ret        = OCI_SUCCESS;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, OCI_DPR_ERROR);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_CONVERTED, OCI_DPR_ERROR);

    dp->nb_processed = 0;

    ret = OCIDirPathLoadStream(dp->ctx, dp->strm, dp->con->err);

    switch (ret)
    {
        case OCI_SUCCESS:
        {
            res                 = OCI_DPR_COMPLETE;

            dp->nb_loaded      += dp->nb_converted;
            dp->nb_processed    = dp->nb_converted;
            dp->status          = OCI_DPS_PREPARED;
            dp->nb_err          = 0;
            dp->idx_err_row     = 0;
            dp->idx_err_col     = 0;

            break;
        }
        case OCI_ERROR:
        {
            res = OCI_DPR_ERROR;

            OCI_ExceptionOCI(dp->con->err, dp->con, NULL, FALSE);

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
        ub4 nb_loaded = 0;
        ub4 size      = sizeof(nb_loaded);

        OCIAttrGet(dp->strm, OCI_HTYPE_DIRPATH_STREAM, &nb_loaded,
                   &size, OCI_ATTR_ROW_COUNT, dp->con->err);

        dp->nb_loaded   += nb_loaded;
        dp->nb_processed = nb_loaded;
    }

     OCI_RESULT(ret == OCI_SUCCESS);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathFinish
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathFinish
(
    OCI_DirPath *dp
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, FALSE);

    OCI_CALL2
    (
        res, dp->con,

        OCIDirPathFinish(dp->ctx, dp->con->err)
    )

    if (res == TRUE)
    {
        dp->status = OCI_DPS_TERMINATED;
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathAbort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathAbort
(
    OCI_DirPath *dp
)
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
    {
        dp->status = OCI_DPS_TERMINATED;
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSave
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSave
(
    OCI_DirPath *dp
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, FALSE);

    OCI_CALL2
    (
        res, dp->con,

        OCIDirPathDataSave(dp->ctx, dp->con->err, OCI_DIRPATH_DATASAVE_SAVEONLY)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathFlushRow
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathFlushRow
(
    OCI_DirPath *dp
)
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

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetCurrentRows
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSetCurrentRows
(
    OCI_DirPath *dp,
    unsigned int nb_rows
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED, FALSE);

    OCI_CHECK_BOUND(dp->con, nb_rows, 1, dp->nb_rows, FALSE);

    dp->nb_cur = (ub2) nb_rows;

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathGetCurrentRows
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathGetCurrentRows
(
    OCI_DirPath *dp
)
{
    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, 0);

    OCI_RESULT(TRUE);

    return dp->nb_cur;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathGetMaxRows
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathGetMaxRows
(
    OCI_DirPath *dp
)
{
    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, 0);

    OCI_RESULT(TRUE);

    return dp->nb_rows;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetDateFormat
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSetDateFormat
(
    OCI_DirPath *dp,
    const mtext *format
)
{
    boolean res = TRUE;
    void *ostr  = NULL;
    int osize   = -1;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED, FALSE);

    osize = -1;
    ostr  = OCI_GetInputMetaString(format, &osize);

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) ostr, (ub4) osize, (ub4) OCI_ATTR_DATEFORMAT, dp->con->err)
    )

    OCI_ReleaseMetaString(ostr);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetParallel
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSetParallel
(
    OCI_DirPath *dp,
    boolean      value
)
{
    boolean res = TRUE;
    ub1 enabled = (ub1) value;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED, FALSE);

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &enabled, (ub4) sizeof(enabled),
                   (ub4) OCI_ATTR_DIRPATH_PARALLEL, dp->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetNoLog
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSetNoLog
(
    OCI_DirPath *dp,
    boolean      value
)
{
    boolean res = TRUE;
    ub1 nolog   = (ub1) value;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED, FALSE);

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &nolog, (ub4) sizeof(nolog),
                   (ub4) OCI_ATTR_DIRPATH_NOLOG, dp->con->err)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetCacheSize
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSetCacheSize
(
    OCI_DirPath *dp,
    unsigned int size
)
{
    boolean res     = TRUE;
    ub4 cache_size  = size;
    boolean enabled = FALSE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_DATE_CACHE_ENABLED(dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_2

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &cache_size, (ub4) sizeof(cache_size),
                   (ub4) OCI_ATTR_DIRPATH_DCACHE_SIZE, dp->con->err)
    )

    OCI_CALL2
    (
        res, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &enabled, (ub4) sizeof(enabled),
                   (ub4) OCI_ATTR_DIRPATH_DCACHE_DISABLE, dp->con->err)
    )

#else

    OCI_NOT_USED(cache_size);
    OCI_NOT_USED(enabled);

#endif

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetBufferSize
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSetBufferSize
(
    OCI_DirPath *dp,
    unsigned int size
)
{
    boolean res = TRUE;
    ub4 bufsize = (ub4) size;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED, FALSE);

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

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetConvertMode
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSetConvertMode
(
    OCI_DirPath *dp,
    unsigned int mode
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    dp->cvt_mode = (ub2) mode;

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathGetRowCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathGetRowCount
(
    OCI_DirPath *dp
)
{
    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_RESULT(TRUE);

    return dp->nb_loaded;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathGetAffectedRows
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathGetAffectedRows
(
    OCI_DirPath *dp
)
{
    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_RESULT(TRUE);

    return dp->nb_processed;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathGetErrorColumn
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathGetErrorColumn
(
    OCI_DirPath *dp
)
{
    ub2 err_col = 0;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_RESULT(TRUE);

    if (dp->idx_err_col < dp->nb_err)
    {
        err_col = dp->err_cols[dp->idx_err_col++];
    }

    return err_col;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathGetErrorRow
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathGetErrorRow
(
    OCI_DirPath *dp
)
{
    ub4 err_row = 0;

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp, FALSE);

    OCI_RESULT(TRUE);

    if (dp->idx_err_row < dp->nb_err)
    {
        err_row = dp->err_rows[dp->idx_err_row++] + 1;
    }

    return err_row;
}
