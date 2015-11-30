/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2015 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
*                             PRIVATE VARIABLES
* ********************************************************************************************* */

static unsigned int ConversionModeValues[] = { OCI_DCM_DEFAULT, OCI_DCM_FORCE };

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetArray
 * --------------------------------------------------------------------------------------------- */

boolean OCI_DirPathSetArray
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

    for (row = row_from; (row < dp->nb_cur) && res; row++)
    {
        for (col = 0; (col < dp->nb_cols) && res; col++)
        {
            OCI_DirPathColumn *dpcol = &(dp->cols[col]);

            /* get internal data cell */

            data = ((ub1 *) dpcol->data) + (size_t) (row * dpcol->bufsize);
            size = dpcol->lens[row];
            flag = dpcol->flags[row];

            if (SQLT_NUM == dpcol->sqlcode)
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

        if (res)
        {
            dp->nb_entries++;
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPahArrayToStream
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_DirPathArrayToStream
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

            if (OCI_DCM_DEFAULT == dp->cvt_mode)
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

    if (OCI_FAILURE(ret))
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

        /* record errors index on real error */
        if (OCI_DPR_ERROR == res)
        {
            dp->err_rows[dp->nb_err] = row_from + err_row;
            dp->err_cols[dp->nb_err] = err_col;

            dp->nb_err++;
        }
    }
    else
    {
        /* conversion is successful. the number of converted rows is the same
           as the number of row set*/
        dp->nb_converted += dp->nb_entries;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPahArrayToStream
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_DirPathLoadStream(OCI_DirPath *dp)
{
    unsigned int res = OCI_DPR_COMPLETE;
    sword ret        = OCI_SUCCESS;
    ub4 nb_loaded    = 0;
    ub4 size         = sizeof(nb_loaded);

    /* load the stream */

    ret = OCIDirPathLoadStream(dp->ctx, dp->strm, dp->con->err);

    switch (ret)
    {
        case OCI_SUCCESS:
        {
            res         = OCI_DPR_COMPLETE;
            dp->status  = OCI_DPS_PREPARED;
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

    /* retrieve the number of rows loaded so far */

    OCIAttrGet(dp->strm, OCI_HTYPE_DIRPATH_STREAM, &nb_loaded,
                &size, OCI_ATTR_ROW_COUNT, dp->con->err);

    dp->nb_loaded    += nb_loaded;
    dp->nb_processed += nb_loaded;

    /* On failure, record errors rows */

    if (OCI_FAILURE(ret))
    {
        dp->err_rows[dp->nb_err] = (dp->nb_err > 0) ? (dp->err_rows[dp->nb_err-1] + nb_loaded + 1) : dp->nb_loaded;
        dp->err_cols[dp->nb_err] = 0;
        dp->nb_err++;
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
    const otext  *partition,
    unsigned int  nb_cols,
    unsigned int  nb_rows
)
{
    OCI_DirPath *dp = NULL;

    OCI_LIB_CALL_ENTER(OCI_DirPath*, dp)

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    OCI_CHECK_COMPAT(typinf->con, typinf->type != OCI_TIF_TYPE)
    OCI_CHECK_BOUND(typinf->con, nb_cols, 1, typinf->nb_cols)

    /* allocate direct path structure */

    dp = (OCI_DirPath *)OCI_MemAlloc(OCI_IPC_DIRPATH, sizeof(*dp), (size_t)1, TRUE);

    if (dp)
    {
        dbtext *dbstr  = NULL;
        int     dbsize = -1;

        dp->con        = typinf->con;
        dp->status     = OCI_DPS_NOT_PREPARED;
        dp->cvt_mode   = OCI_DCM_DEFAULT;
        dp->res_conv   = OCI_DPR_EMPTY;
        dp->res_load   = OCI_DPR_EMPTY;
        dp->typinf     = typinf;
        dp->nb_rows    = (ub2)nb_rows;
        dp->nb_cols    = (ub2)nb_cols;
        dp->nb_cur     = (ub2)dp->nb_rows;

        /* allocates direct context handle */

        call_status = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *)dp->con->env,
                                                     (dvoid **) (void *) &dp->ctx,
                                                     (ub4) OCI_HTYPE_DIRPATH_CTX,
                                                     (size_t) 0, (dvoid **) NULL));

        /* set table name attribute */

        if (call_status)
        {
            dbsize = -1;
            dbstr  = OCI_StringGetOracleString(dp->typinf->name, &dbsize);

            OCI_CALL2
            (
                call_status, dp->con,

                OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                           (dvoid *) dbstr, (ub4) dbsize, (ub4) OCI_ATTR_NAME, dp->con->err)
            )

            OCI_StringReleaseOracleString(dbstr);
        }

        /* set schema name attribute */

        if (call_status && dp->typinf->schema && dp->typinf->schema[0])
        {
            dbsize = -1;
            dbstr  = OCI_StringGetOracleString(dp->typinf->schema, &dbsize);

            OCI_CALL2
            (
                call_status, dp->con,

                OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                           (dvoid *) dbstr, (ub4) dbsize, (ub4) OCI_ATTR_SCHEMA_NAME,
                           dp->con->err)
            )

            OCI_StringReleaseOracleString(dbstr);
        }

        /* set partition name attribute */

        if (call_status && partition && partition[0])
        {
            dbsize = -1;
            dbstr  = OCI_StringGetOracleString(partition, &dbsize);

            OCI_CALL2
            (
                call_status, dp->con,

                OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                           (dvoid *) dbstr, (ub4) dbsize, (ub4) OCI_ATTR_SUB_NAME,
                           dp->con->err)
            )

            OCI_StringReleaseOracleString(dbstr);
        }

        if (OCILib.version_runtime >= OCI_9_0)
        {
            ub4 num_rows = dp->nb_rows;

            /* set array size attribute */

            OCI_CALL2
            (
                call_status, dp->con,

                OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                           (dvoid *) &num_rows, (ub4) sizeof(num_rows),
                           (ub4) OCI_ATTR_NUM_ROWS, dp->con->err)
            )
        }

        /* set columns count attribute */

        OCI_CALL2
        (
            call_status, dp->con,

            OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                       (dvoid *) &dp->nb_cols, (ub4) sizeof(dp->nb_cols),
                       (ub4) OCI_ATTR_NUM_COLS, dp->con->err)
        )

        /* allocating the column array */

        if (call_status)
        {
            dp->cols = (OCI_DirPathColumn *) OCI_MemAlloc(OCI_IPC_DP_COL_ARRAY,
                                                          sizeof(OCI_DirPathColumn),
                                                          (size_t) dp->nb_cols,
                                                          TRUE);

            call_status = (NULL != dp->cols);
        }
    }

    /* handle errors */

    if (call_status)
    {
        call_retval = dp;
    }
    else if (dp)
    {
        OCI_DirPathFree(dp);
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathFree
(
    OCI_DirPath *dp
)
{
    ub2 i = 0;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)

    for (i = 0; i < dp->nb_cols; i++)
    {
        OCI_FREE(dp->cols[i].data)
        OCI_FREE(dp->cols[i].lens)
        OCI_FREE(dp->cols[i].flags)
        OCI_FREE(dp->cols[i].format)
    }

    OCI_FREE(dp->cols)
    OCI_FREE(dp->err_cols)
    OCI_FREE(dp->err_rows)

    OCI_HandleFree(dp->strm, OCI_HTYPE_DIRPATH_STREAM);
    OCI_HandleFree(dp->arr,  OCI_HTYPE_DIRPATH_COLUMN_ARRAY);
    OCI_HandleFree(dp->ctx,  OCI_HTYPE_DIRPATH_CTX);

    OCI_FREE(dp)

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetColumn
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSetColumn
(
    OCI_DirPath *dp,
    unsigned int index,
    const otext *name,
    unsigned int maxsize,
    const otext *format
)
{
    OCI_DirPathColumn *dpcol  = NULL;
    OCI_Column        *col    = NULL;
    OCIParam          *hattr  = NULL;
    OCIParam          *hlist  = NULL;
    dbtext            *dbstr  = NULL;
    int                dbsize = -1;

    ub2 i = 0;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    OCI_CHECK_PTR(OCI_IPC_STRING, name)
    OCI_CHECK_BOUND(dp->con, index, 1, dp->nb_cols)

    call_status = TRUE;

    /* check if column exists */

    for (i = 0; i < dp->typinf->nb_cols; i++)
    {
        if (0 == ostrcasecmp(name, dp->typinf->cols[i].name))
        {
            break;
        }
    }

    /* check if column was found */

    if (i >= dp->typinf->nb_cols)
    {
        OCI_ExceptionDirPathColNotFound(dp, name, dp->typinf->name);

        call_status = FALSE;
    }

    /* set column information */

    if (call_status)
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

        switch (col->datatype)
        {
            case OCI_CDT_TEXT:
            {
                dpcol->maxsize *= sizeof(otext);
                dpcol->bufsize *= sizeof(otext);

                if (OCILib.nls_utf8)
                {
                    dpcol->bufsize *= UTF8_BYTES_PER_CHAR;
                }
                break;
            }
            case OCI_CDT_NUMERIC:
            {
                if (format && format[0])
                {
                    dpcol->format      = ostrdup(format);
                    dpcol->format_size = (ub4) ostrlen(format);
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

                if (format && format[0])
                {
                    dpcol->format      = ostrdup(format);
                    dpcol->format_size = (ub4) ostrlen(format);
                    dpcol->maxsize     = (ub2) dpcol->format_size;
                    dpcol->bufsize    *= sizeof(otext);
                }
                break;
            }
            case OCI_CDT_LOB:
            {
                if (OCI_BLOB == col->subtype)
                {
                    dpcol->type    = OCI_DDT_BINARY;
                    dpcol->sqlcode = SQLT_BIN;
                }

                break;
            }
            case OCI_CDT_LONG:
            {
                if (OCI_BLONG == col->subtype)
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
                call_status = FALSE;
                OCI_ExceptionDatatypeNotSupported(dp->con, NULL, col->libcode);
                break;
            }
        }
    }

    /* if supported data type, set direct path column attributes */

    if (call_status)
    {
        /* get column parameter list handle */

        OCI_CALL2
        (
            call_status, dp->con,

            OCIAttrGet(dp->ctx, OCI_HTYPE_DIRPATH_CTX, &hlist, NULL, OCI_ATTR_LIST_COLUMNS, dp->con->err)
        )

        /* get column attribute handle */

        OCI_CALL2
        (
            call_status, dp->con,

            OCIParamGet((dvoid *) hlist, OCI_DTYPE_PARAM, dp->con->err,
                        (dvoid** ) (dvoid *) &hattr, (ub4) index)
        )

        /* set column name */

        if (call_status)
        {
            dbsize = -1;
            dbstr  = OCI_StringGetOracleString(name, &dbsize);

            OCI_CALL2
            (
                call_status, dp->con,

                OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                           (dvoid *) dbstr, (ub4) dbsize, (ub4) OCI_ATTR_NAME, dp->con->err)
            )

            OCI_StringReleaseOracleString(dbstr);
        }

        /* set column type */

        OCI_CALL2
        (
            call_status, dp->con,

            OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) &dpcol->sqlcode, sizeof(dpcol->sqlcode),
                       (ub4) OCI_ATTR_DATA_TYPE, dp->con->err)
        )

        /* set column size */

        OCI_CALL2
        (
            call_status, dp->con,

            OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) &dpcol->maxsize, sizeof(dpcol->maxsize),
                       (ub4) OCI_ATTR_DATA_SIZE, dp->con->err)
        )

        /* set column precision */

        if (col->prec != 0)
        {
            OCI_CALL2
            (
                call_status, dp->con,

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
                call_status, dp->con,

                OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                           (dvoid *) &col->scale, sizeof(col->scale),
                           (ub4) OCI_ATTR_SCALE, dp->con->err)
            )
        }

        /* set column date/time format attribute */

        if (call_status && dpcol->format && dpcol->format[0] && (OCI_DDT_NUMBER != dpcol->type))
        {
            dbsize = -1;
            dbstr  = OCI_StringGetOracleString(dpcol->format, &dbsize);

            OCI_CALL2
            (
                call_status, dp->con,

                OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                           (dvoid *) dbstr, (ub4) dbsize,
                           (ub4) OCI_ATTR_DATEFORMAT, dp->con->err)
            )

            OCI_StringReleaseOracleString(dbstr);
        }

        /* setup Unicode mode for Unicode user data */

        if (OCI_DDT_TEXT == dpcol->type && OCI_CHAR_WIDE == OCILib.charset)
        {
            ub2 csid = OCI_UTF16ID;

            OCI_CALL2
            (
                call_status, dp->con,

                OCIAttrSet((dvoid *) hattr, (ub4) OCI_DTYPE_PARAM,
                           (dvoid *) &csid,  (ub4) sizeof(csid),
                           (ub4) OCI_ATTR_CHARSET_ID,  dp->con->err)
            )
        }

        /* free param handle */

        OCIDescriptorFree(hattr, OCI_DTYPE_PARAM);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathPrepare
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathPrepare
(
    OCI_DirPath *dp
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)

    call_status = TRUE;

    /* prepare direct path operation */

    OCI_CALL2
    (
        call_status, dp->con,

        OCIDirPathPrepare(dp->ctx, dp->con->cxt, dp->con->err)
    )

    /* allocate column array handle */

    if (call_status)
    {
        call_status = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *)dp->ctx,
                                                     (dvoid **) (void *) &dp->arr,
                                                     (ub4) OCI_HTYPE_DIRPATH_COLUMN_ARRAY,
                                                     (size_t) 0, (dvoid **) NULL));
    }

    /* allocate stream handle */

    if (call_status)
    {
        call_status = OCI_SUCCESSFUL(OCI_HandleAlloc((dvoid *)dp->ctx,
                                                     (dvoid **) (void *) &dp->strm,
                                                     (ub4) OCI_HTYPE_DIRPATH_STREAM,
                                                     (size_t) 0, (dvoid **) NULL));
    }

    /* check the number of rows allocated */

    if (call_status)
    {
        ub4 num_rows = 0;
        ub4 size     = sizeof(num_rows);

        OCI_CALL2
        (
            call_status, dp->con,

            OCIAttrGet(dp->arr, OCI_HTYPE_DIRPATH_COLUMN_ARRAY, &num_rows,
                       &size, OCI_ATTR_NUM_ROWS, dp->con->err)
        )

        dp->nb_cur  = (ub2) num_rows;
        dp->nb_rows = (ub2) num_rows;
    }

    /* allocate array of errs rows */

    if (call_status)
    {
        dp->err_rows = (ub4 *) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, sizeof(*dp->err_rows),
                                            (size_t) dp->nb_cur, TRUE);

        call_status = (NULL != dp->err_rows);
    }

    /* allocate array of errs cols */

    if (call_status)
    {
        dp->err_cols = (ub2 *) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, sizeof(*dp->err_cols),
                                            (size_t) dp->nb_cur, TRUE);

        call_status = (NULL != dp->err_cols);
    }

    /* now, we need to allocate internal buffers */

    if (call_status)
    {
        ub2 i;

        for (i = 0; i < dp->nb_cols; i++)
        {
            OCI_DirPathColumn *col = &dp->cols[i];

            /* data buffers */

            col->data = (ub1 *) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY,
                                             (size_t) col->bufsize,
                                             (size_t) dp->nb_cur, TRUE);

            if (!col->data)
            {
                call_status = FALSE;
                break;
            }

            /* data sizes */

            col->lens = (ub4 *) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, sizeof(ub4),
                                             (size_t) dp->nb_cur, TRUE);

            if (!col->lens)
            {
                call_status = FALSE;
                break;
            }

            /* data flags */

            col->flags = (ub1 *) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, sizeof(ub1),
                                              (size_t) dp->nb_cur, TRUE);

            if (!col->flags)
            {
                call_status = FALSE;
                break;
            }
        }
    }

    if (call_status)
    {
        dp->status = OCI_DPS_PREPARED;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_DirPathColumn *dpcol = NULL;

    ub1 *data;
    ub1 flag;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)

    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)
    OCI_CHECK_BOUND(dp->con, index, 1, dp->nb_cols)
    OCI_CHECK_BOUND(dp->con, row, 1, dp->nb_cur)

    dpcol = &dp->cols[index-1];

    if (dpcol)
    {
        call_status = TRUE;

        /* check size */

        if (size > dpcol->maxsize)
        {
            size = (unsigned int) dpcol->maxsize;
        }

        /* setup column flag */

        if (!value)
        {
            flag = OCI_DIRPATH_COL_NULL;
            size = 0;
        }
        else if (complete)
        {
            flag = OCI_DIRPATH_COL_COMPLETE;
        }
        else
        {
            flag = OCI_DIRPATH_COL_PARTIAL;
        }

        /* Process only if data is not null */

        if (value)
        {
            /* for character based column, parameter size was the number of characters */

            if (SQLT_CHR == dpcol->sqlcode)
            {
                size *= (unsigned int) sizeof(otext);
            }

            /* get internal data cell */

            data = ((ub1 *) dpcol->data) + (size_t) ((row-1) * dpcol->bufsize);

            /* we weed to pack the buffer if wchar_t is 4 bytes */

            if (OCI_DDT_TEXT == dpcol->type && OCILib.use_wide_char_conv)
            {
                size = ocharcount(size);
                OCI_StringUTF32ToUTF16(value, data, size);
            }
            else if (OCI_DDT_OTHERS == dpcol->type && OCI_CHAR_WIDE == OCILib.charset)
            {
                /* input Unicode numeric values causes oracle conversion error.
                   so, let's convert them to ANSI */

                size = ocharcount(size);
                OCI_StringNativeToAnsi(value, data, size);
            }
            else if (OCI_DDT_NUMBER == dpcol->type)
            {
               /* if a format was provided for a numeric column, we convert the input
               buffer to a OCINumber */

                OCINumber *num = (OCINumber *) data;

                call_status = OCI_NumberFromString(dp->con, num, sizeof(*num), OCI_NUM_NUMBER,
                                                   SQLT_VNU, (dtext *)value, dpcol->format);

                if (call_status)
                {
                    size = (unsigned int) num->OCINumberPart[0];
                }
            }
            else
            {
                memcpy(data, value, (size_t) size);
            }
        }

        dpcol->lens[row-1]  = size;
        dpcol->flags[row-1] = flag;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathReset
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathReset
(
    OCI_DirPath *dp
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)

    call_status = TRUE;

    /* reset conversion and loading variables */

    dp->nb_processed    = 0;
    dp->nb_converted    = 0;
    dp->nb_err          = 0;
    dp->idx_err_row     = 0;
    dp->idx_err_col     = 0;

    /* reset array */

    OCI_CALL2
    (
        call_status, dp->con,

        OCIDirPathColArrayReset(dp->arr, dp->con->err)
    )

    /* reset stream */

    OCI_CALL2
    (
        call_status, dp->con,

        OCIDirPathStreamReset(dp->strm, dp->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathConvert
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathConvert
(
    OCI_DirPath *dp
)
{
    ub4 row_from = 0;

    OCI_LIB_CALL_ENTER(unsigned int, OCI_DPR_ERROR)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)

    call_status = TRUE;

    /* reset the number of processed rows */

    dp->nb_processed = 0;

    /* in case of previous error in default mode or if the stream is full,
       let's start again from the last faulted row */

    if ((OCI_DCM_DEFAULT == dp->cvt_mode || OCI_DPR_FULL == dp->res_conv) && (dp->nb_err > 0))
    {
        row_from = dp->err_rows[dp->nb_err - 1];
    }

    /* reset the stream if it is full */

    if (OCI_DPR_FULL == dp->res_conv)
    {
        OCI_CALL2
        (
            call_status, dp->con,

            OCIDirPathStreamReset(dp->strm, dp->con->err)
        )
    }

    /* reset conversion status back to default error value */

    dp->res_conv = OCI_DPR_ERROR;

    /* set array values */

    if (call_status && OCI_DirPathSetArray(dp, row_from))
    {
        /* try to convert values from array into stream */

        dp->res_conv = OCI_DirPathArrayToStream(dp, row_from);

        /* in case of conversion error, continue conversion in force mode
           other return from conversion */

        if (OCI_DCM_FORCE == dp->cvt_mode && OCI_DPR_ERROR == dp->res_conv)
        {
            /* perform conversion until all non erred rows are converted */

            while (call_status && (OCI_DPR_ERROR == dp->res_conv) && (dp->nb_err <= dp->nb_cur))
            {
                /* start from the row that follows the last erred row */

                row_from = dp->err_rows[dp->nb_err - 1] + 1;

                /* set values again */

                call_status = OCI_DirPathSetArray(dp, row_from);

                if (call_status)
                {
                     /* perform conversion again */

                     dp->res_conv = OCI_DirPathArrayToStream(dp, row_from);
                }
            }
        }
    }

    dp->nb_processed = dp->nb_converted;

    call_status = call_status && (OCI_DPR_COMPLETE == dp->res_conv);
    call_retval = dp->res_conv;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathLoad
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathLoad
(
    OCI_DirPath *dp
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_DPR_ERROR)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_CONVERTED)

    call_status = TRUE;

    /* reset the number of processed rows */

    dp->nb_processed = 0;

   /* reset errors variables as OCI_DirPathLoad() is not reentrant */

    dp->nb_err       = 0;
    dp->idx_err_col  = 0;
    dp->idx_err_row  = 0;
    dp->res_load     = OCI_DPR_COMPLETE;

    /* load the stream */

    dp->res_load = OCI_DirPathLoadStream(dp);

    /* continue to load the stream while it returns an error */

    while (OCI_DPR_ERROR == dp->res_load)
    {
        dp->res_load = OCI_DirPathLoadStream(dp);
    }

    call_status = call_status && (OCI_DPR_COMPLETE == dp->res_load);
    call_retval = dp->res_load;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathFinish
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathFinish
(
    OCI_DirPath *dp
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dp->con,

        OCIDirPathFinish(dp->ctx, dp->con->err)
    )

    if (call_status)
    {
        dp->status = OCI_DPS_TERMINATED;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathAbort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathAbort
(
    OCI_DirPath *dp
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dp->con,

        OCIDirPathAbort(dp->ctx, dp->con->err)
    )

    if (call_status)
    {
        dp->status = OCI_DPS_TERMINATED;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSave
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSave
(
    OCI_DirPath *dp
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dp->con,

        OCIDirPathDataSave(dp->ctx, dp->con->err, OCI_DIRPATH_DATASAVE_SAVEONLY)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathFlushRow
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathFlushRow
(
    OCI_DirPath *dp
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dp->con,

        OCIDirPathFlushRow(dp->ctx, dp->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)
    OCI_CHECK_BOUND(dp->con, nb_rows, 1, dp->nb_rows)

    dp->nb_cur = (ub2) nb_rows;

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathGetCurrentRows
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathGetCurrentRows
(
    OCI_DirPath *dp
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)

    call_retval = dp->nb_cur;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathGetMaxRows
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathGetMaxRows
(
    OCI_DirPath *dp
)
{
   OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)

    call_retval = dp->nb_rows;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathSetDateFormat
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DirPathSetDateFormat
(
    OCI_DirPath *dp,
    const otext *format
)
{
    dbtext  *dbstr = NULL;
    int     dbsize = -1;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)

    call_status = TRUE;

    dbsize = -1;
    dbstr  = OCI_StringGetOracleString(format, &dbsize);

    OCI_CALL2
    (
        call_status, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) dbstr, (ub4) dbsize, (ub4) OCI_ATTR_DATEFORMAT, dp->con->err)
    )

    OCI_StringReleaseOracleString(dbstr);

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    ub1 enabled = (ub1) value;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &enabled, (ub4) sizeof(enabled),
                   (ub4) OCI_ATTR_DIRPATH_PARALLEL, dp->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    ub1 nolog = (ub1) value;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &nolog, (ub4) sizeof(nolog),
                   (ub4) OCI_ATTR_DIRPATH_NOLOG, dp->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    ub4 cache_size  = size;
    boolean enabled = FALSE;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)

    call_status = TRUE;

#if OCI_VERSION_COMPILE >= OCI_9_2

    OCI_CALL2
    (
        call_status, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &cache_size, (ub4) sizeof(cache_size),
                   (ub4) OCI_ATTR_DIRPATH_DCACHE_SIZE, dp->con->err)
    )

    OCI_CALL2
    (
        call_status, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &enabled, (ub4) sizeof(enabled),
                   (ub4) OCI_ATTR_DIRPATH_DCACHE_DISABLE, dp->con->err)
    )

#else

    OCI_NOT_USED(cache_size)
    OCI_NOT_USED(enabled)

#endif

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    ub4 bufsize = (ub4) size;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, dp->con,

        OCIAttrSet((dvoid *) dp->ctx, (ub4) OCI_HTYPE_DIRPATH_CTX,
                   (dvoid *) &bufsize, (ub4) sizeof(bufsize),
                   (ub4) OCI_ATTR_BUF_SIZE, dp->con->err)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    OCI_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    OCI_CHECK_ENUM_VALUE(dp->con, NULL, mode, ConversionModeValues, OTEXT("Conversion mode"))

    dp->cvt_mode = (ub2) mode;

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathGetRowCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathGetRowCount
(
    OCI_DirPath *dp
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)

    call_retval = dp->nb_loaded;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathGetAffectedRows
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathGetAffectedRows
(
    OCI_DirPath *dp
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)

    call_retval = dp->nb_processed;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathGetErrorColumn
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathGetErrorColumn
(
    OCI_DirPath *dp
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)

    if (dp->idx_err_col < dp->nb_err)
    {
        call_retval = (unsigned int) dp->err_cols[dp->idx_err_col++] + 1;
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DirPathGetErrorRow
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_DirPathGetErrorRow
(
    OCI_DirPath *dp
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_DIRPATH, dp)

    if (dp->idx_err_row < dp->nb_err)
    {
        call_retval = (unsigned int) dp->err_rows[dp->idx_err_row++] + 1;
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}
