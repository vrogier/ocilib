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

#include "dirpath.h"

#include "macros.h"
#include "memory.h"
#include "number.h"
#include "strings.h"

static const unsigned int ConversionModeValues[] = { OCI_DCM_DEFAULT, OCI_DCM_FORCE };

/* --------------------------------------------------------------------------------------------- *
 * DirPathSetArray
 * --------------------------------------------------------------------------------------------- */

boolean DirPathSetArray
(
    OCI_DirPath *dp,
    ub4 row_from
)
{
    ub1     *data    = NULL;
    ub4      size    = 0;
    ub1      flag    = 0;
    ub2      col     = 0;
    ub4      row     = 0;

    DECLARE_CTX(TRUE)
  
    CHECK(NULL == dp, FALSE)

    CALL_CONTEXT_FROM_CON(dp->con)

    /* reset the number of entries et */

    dp->nb_entries = 0;

    /* set entries */

    for (row = row_from; (row < dp->nb_cur) && STATUS; row++)
    {
        for (col = 0; (col < dp->nb_cols) && STATUS; col++)
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

            EXEC(OCIDirPathColArrayEntrySet(dp->arr, dp->con->err, (ub4) dp->nb_entries,
                                                (ub2) (col), (ub1*) data, (ub4) size, flag))
        }

        /* increment number of item set */

        if (STATUS)
        {
            dp->nb_entries++;
        }
    }

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * DirPahArrayToStream
 * --------------------------------------------------------------------------------------------- */

unsigned int DirPathArrayToStream
(
    OCI_DirPath *dp,
    ub4 row_from
)
{
    unsigned int res = OCI_DPR_COMPLETE;
    sword        ret = OCI_SUCCESS;

    DECLARE_CTX(TRUE)

    CHECK(NULL == dp, OCI_DPR_ERROR)

    CALL_CONTEXT_FROM_CON(dp->con)

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
                ExceptionOCI(dp->con->err, dp->con, NULL, FALSE);
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

        ATTRIB_GET(OCI_HTYPE_DIRPATH_COLUMN_ARRAY, OCI_ATTR_COL_COUNT, dp->arr, &err_col, &size)

        size = sizeof(err_row);

        ATTRIB_GET(OCI_HTYPE_DIRPATH_COLUMN_ARRAY, OCI_ATTR_ROW_COUNT, dp->arr, &err_row, &size)

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
 * DirPahArrayToStream
 * --------------------------------------------------------------------------------------------- */

unsigned int DirPathLoadStream(OCI_DirPath *dp)
{       
    unsigned int res = OCI_DPR_COMPLETE;
    sword ret        = OCI_SUCCESS;
    ub4 nb_loaded    = 0;
    ub4 size         = sizeof(nb_loaded);

    DECLARE_CTX(TRUE)

    CHECK(NULL == dp, OCI_DPR_ERROR)

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

            ExceptionOCI(dp->con->err, dp->con, NULL, FALSE);
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

    ATTRIB_GET(OCI_HTYPE_DIRPATH_STREAM, OCI_ATTR_ROW_COUNT, dp->strm, &nb_loaded, &size)

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

/* --------------------------------------------------------------------------------------------- *
 * DirPathCreate
 * --------------------------------------------------------------------------------------------- */

OCI_DirPath * DirPathCreate
(
    OCI_TypeInfo *typinf,
    const otext  *partition,
    unsigned int  nb_cols,
    unsigned int  nb_rows
)
{
    OCI_DirPath *dp = NULL;

    CALL_ENTER(OCI_DirPath*, dp)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CALL_CHECK_COMPAT(typinf->con, typinf->type != OCI_TIF_TYPE)
    CALL_CHECK_BOUND(typinf->con, nb_cols, 1, typinf->nb_cols)
    CALL_CONTEXT_FROM_CON(typinf->con)

    /* allocate direct path structure */

    ALLOC_DATA(OCI_IPC_DIRPATH, dp, 1)

    if (STATUS)
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

        STATUS = MemoryAllocHandle((dvoid *)dp->con->env, (dvoid **) (void *) &dp->ctx, OCI_HTYPE_DIRPATH_CTX);

        /* set table name attribute */

        if (STATUS)
        {
            dbsize = -1;
            dbstr  = StringGetDBString(dp->typinf->name, &dbsize);

            ATTRIB_SET(OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_NAME, dp->ctx, dbstr, dbsize)

            StringReleaseDBString(dbstr);
        }

        /* set schema name attribute */

        if (STATUS && IS_STRING_VALID(dp->typinf->schema))
        {
            dbsize = -1;
            dbstr  = StringGetDBString(dp->typinf->schema, &dbsize);

            ATTRIB_SET(OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_SCHEMA_NAME, dp->ctx, dbstr, dbsize)

            StringReleaseDBString(dbstr);
        }

        /* set partition name attribute */

        if (STATUS && IS_STRING_VALID(partition))
        {
            dbsize = -1;
            dbstr  = StringGetDBString(partition, &dbsize);

            ATTRIB_SET(OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_SUB_NAME, dp->ctx, dbstr, dbsize)

            StringReleaseDBString(dbstr);
        }

        if (Env.version_runtime >= OCI_9_0)
        {
            ub4 num_rows = dp->nb_rows;

            /* set array size attribute */

            ATTRIB_SET(OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_NUM_ROWS, dp->ctx, &num_rows, sizeof(num_rows))
        }

        /* set columns count attribute */

        ATTRIB_SET(OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_NUM_COLS, dp->ctx, &dp->nb_cols, sizeof(dp->nb_cols))

        /* allocating the column array */

        ALLOC_DATA(OCI_IPC_DP_COL_ARRAY, dp->cols, dp->nb_cols)
    }

    /* handle errors */

    if (STATUS)
    {
        RETVAL = dp;
    }
    else if (dp)
    {
        DirPathFree(dp);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathFree
 * --------------------------------------------------------------------------------------------- */

boolean DirPathFree
(
    OCI_DirPath *dp
)
{
    ub2 i = 0;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CONTEXT_FROM_CON(dp->con)

    for (i = 0; i < dp->nb_cols; i++)
    {
        FREE(dp->cols[i].data)
        FREE(dp->cols[i].lens)
        FREE(dp->cols[i].flags)
        FREE(dp->cols[i].format)
    }

    FREE(dp->cols)
    FREE(dp->err_cols)
    FREE(dp->err_rows)

    MemoryFreeHandle(dp->strm, OCI_HTYPE_DIRPATH_STREAM);
    MemoryFreeHandle(dp->arr,  OCI_HTYPE_DIRPATH_COLUMN_ARRAY);
    MemoryFreeHandle(dp->ctx,  OCI_HTYPE_DIRPATH_CTX);

    FREE(dp)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathSetColumn
 * --------------------------------------------------------------------------------------------- */

boolean DirPathSetColumn
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

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    CALL_CHECK_PTR(OCI_IPC_STRING, name)
    CALL_CHECK_BOUND(dp->con, index, 1, dp->nb_cols)
    CALL_CONTEXT_FROM_CON(dp->con)

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
        THROW(ExceptionDirPathColNotFound(dp, name, dp->typinf->name))
    }

    /* set column information */

    if (STATUS)
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

                if (Env.nls_utf8)
                {
                    dpcol->bufsize *= OCI_UTF8_BYTES_PER_CHAR;
                }
                break;
            }
            case OCI_CDT_NUMERIC:
            {
                if (IS_STRING_VALID(format))
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

                if (IS_STRING_VALID(format))
                {
                    dpcol->format      = ostrdup(format);
                    dpcol->format_size = (ub4) ostrlen(format);
                    dpcol->maxsize     = (ub2) max(dpcol->format_size, maxsize);
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
                THROW(ExceptionDatatypeNotSupported(dp->con, NULL, col->libcode))
            }
        }
    }

    /* if supported data type, set direct path column attributes */

    if (STATUS)
    {
        /* get column parameter list handle */

        ATTRIB_GET(OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_LIST_COLUMNS, dp->ctx, &hlist, NULL)

        /* get column attribute handle */

        EXEC(OCIParamGet((dvoid *) hlist, OCI_DTYPE_PARAM, dp->con->err, (dvoid** ) (dvoid *) &hattr, (ub4) index))

        /* set column name */

        if (STATUS)
        {
            dbsize = -1;
            dbstr  = StringGetDBString(name, &dbsize);

            ATTRIB_SET(OCI_DTYPE_PARAM, OCI_ATTR_NAME, hattr, dbstr, dbsize)

            StringReleaseDBString(dbstr);
        }

        /* set column type */

        ATTRIB_SET(OCI_DTYPE_PARAM, OCI_ATTR_DATA_TYPE, hattr, &dpcol->sqlcode, sizeof(dpcol->sqlcode))

        /* set column size */

        ATTRIB_SET(OCI_DTYPE_PARAM, OCI_ATTR_DATA_SIZE, hattr, &dpcol->maxsize, sizeof(dpcol->maxsize))

        /* set column precision */

        if (col->prec != 0)
        {
            ATTRIB_SET(OCI_DTYPE_PARAM, OCI_ATTR_PRECISION, hattr, &col->prec, sizeof(col->prec))
        }

        /* set column scale */

        if (col->scale != 0)
        {
            ATTRIB_SET(OCI_DTYPE_PARAM, OCI_ATTR_SCALE, hattr, &col->scale, sizeof(col->scale))
        }

        /* set column date/time format attribute */

        if (STATUS && dpcol->format && dpcol->format[0] && (OCI_DDT_NUMBER != dpcol->type))
        {
            dbsize = -1;
            dbstr  = StringGetDBString(dpcol->format, &dbsize);

            ATTRIB_SET(OCI_DTYPE_PARAM, OCI_ATTR_DATEFORMAT, hattr, dbstr, dbsize)

            StringReleaseDBString(dbstr);
        }

        /* setup Unicode mode for Unicode user data */

        if (OCI_DDT_TEXT == dpcol->type && OCI_CHAR_WIDE == Env.charset)
        {
            ub2 csid = OCI_UTF16ID;

            ATTRIB_SET(OCI_DTYPE_PARAM, OCI_ATTR_CHARSET_ID, hattr, &csid, sizeof(csid))
        }

        /* free param handle */

        OCIDescriptorFree(hattr, OCI_DTYPE_PARAM);
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathPrepare
 * --------------------------------------------------------------------------------------------- */

boolean DirPathPrepare
(
    OCI_DirPath *dp
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    CALL_CONTEXT_FROM_CON(dp->con)

    /* prepare direct path operation */

    EXEC(OCIDirPathPrepare(dp->ctx, dp->con->cxt, dp->con->err))

    /* allocate column array handle */

    STATUS = STATUS &&  MemoryAllocHandle((dvoid *)dp->ctx, (dvoid **)(void *)&dp->arr, OCI_HTYPE_DIRPATH_COLUMN_ARRAY);

    /* allocate stream handle */

    STATUS = STATUS && MemoryAllocHandle((dvoid *)dp->ctx, (dvoid **)(void *)&dp->strm, OCI_HTYPE_DIRPATH_STREAM);

    /* check the number of rows allocated */

    if (STATUS)
    {
        ub4 num_rows = 0;
        ub4 size     = sizeof(num_rows);

        ATTRIB_GET(OCI_HTYPE_DIRPATH_COLUMN_ARRAY, OCI_ATTR_NUM_ROWS, dp->arr, &num_rows, &size)

        dp->nb_cur  = (ub2) num_rows;
        dp->nb_rows = (ub2) num_rows;
    }

    /* allocate array of errs rows */

    ALLOC_DATA(OCI_IPC_BUFF_ARRAY, dp->err_rows, dp->nb_cur)


    /* allocate array of errs cols */

    ALLOC_DATA(OCI_IPC_BUFF_ARRAY, dp->err_cols, dp->nb_cur)

    /* now, we need to allocate internal buffers */

    if (STATUS)
    {
        for (ub2 i = 0; i < dp->nb_cols && STATUS; i++)
        {
            OCI_DirPathColumn *col = &dp->cols[i];

            ALLOC_BUFFER(OCI_IPC_BUFF_ARRAY, col->data, col->bufsize, dp->nb_cur)
            ALLOC_BUFFER(OCI_IPC_BUFF_ARRAY, col->lens, sizeof(ub4),  dp->nb_cur)
            ALLOC_BUFFER(OCI_IPC_BUFF_ARRAY, col->flags, sizeof(ub1), dp->nb_cur)
        }
    }

    if (STATUS)
    {
        dp->status = OCI_DPS_PREPARED;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathSetEntry
 * --------------------------------------------------------------------------------------------- */

boolean DirPathSetEntry
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

    ub1 flag = 0;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)
    CALL_CHECK_BOUND(dp->con, index, 1, dp->nb_cols)
    CALL_CHECK_BOUND(dp->con, row, 1, dp->nb_cur)
    CALL_CONTEXT_FROM_CON(dp->con)

    dpcol = &dp->cols[index-1];
    STATUS = (NULL != dpcol);

    if (STATUS)
    {
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

            ub1 *data = ((ub1 *) dpcol->data) + (size_t) ((row-1) * dpcol->bufsize);

            /* we weed to pack the buffer if wchar_t is 4 bytes */

            if (OCI_DDT_TEXT == dpcol->type && Env.use_wide_char_conv)
            {
                size = ocharcount(size);
                StringUTF32ToUTF16(value, data, size);
            }
            else if (OCI_DDT_OTHERS == dpcol->type && OCI_CHAR_WIDE == Env.charset)
            {
                /* input Unicode numeric values causes oracle conversion error.
                   so, let's convert them to ANSI */

                size = ocharcount(size);
                StringNativeToAnsi(value, data, size);
            }
            else if (OCI_DDT_NUMBER == dpcol->type)
            {
               /* if a format was provided for a numeric column, we convert the input
               buffer to a OCINumber */

                OCINumber *num = (OCINumber *) data;

                STATUS = NumberFromStringInternal(dp->con, num, OCI_NUM_NUMBER, (otext *)value, dpcol->format);

                if (STATUS)
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

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathReset
 * --------------------------------------------------------------------------------------------- */

boolean DirPathReset
(
    OCI_DirPath *dp
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CONTEXT_FROM_CON(dp->con)

    /* reset conversion and loading variables */

    dp->nb_processed    = 0;
    dp->nb_converted    = 0;
    dp->nb_err          = 0;
    dp->idx_err_row     = 0;
    dp->idx_err_col     = 0;

    /* reset array */

    EXEC(OCIDirPathColArrayReset(dp->arr, dp->con->err))

    /* reset stream */

    EXEC(OCIDirPathStreamReset(dp->strm, dp->con->err))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathConvert
 * --------------------------------------------------------------------------------------------- */

unsigned int DirPathConvert
(
    OCI_DirPath *dp
)
{
    ub4 row_from = 0;

    CALL_ENTER(unsigned int, OCI_DPR_ERROR)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)
    CALL_CONTEXT_FROM_CON(dp->con)

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
        EXEC(OCIDirPathStreamReset(dp->strm, dp->con->err))
    }

    /* reset conversion status back to default error value */

    dp->res_conv = OCI_DPR_ERROR;

    /* set array values */

    if (STATUS && DirPathSetArray(dp, row_from))
    {
        /* try to convert values from array into stream */

        dp->res_conv = DirPathArrayToStream(dp, row_from);

        /* in case of conversion error, continue conversion in force mode
           other return from conversion */

        if (OCI_DCM_FORCE == dp->cvt_mode && OCI_DPR_ERROR == dp->res_conv)
        {
            /* perform conversion until all non erred rows are converted */

            while (STATUS && (OCI_DPR_ERROR == dp->res_conv) && (dp->nb_err <= dp->nb_cur))
            {
                /* start from the row that follows the last erred row */

                row_from = dp->err_rows[dp->nb_err - 1] + 1;

                /* set values again */

                STATUS = DirPathSetArray(dp, row_from);

                if (STATUS)
                {
                     /* perform conversion again */

                     dp->res_conv = DirPathArrayToStream(dp, row_from);
                }
            }
        }
    }

    dp->nb_processed = dp->nb_converted;

    STATUS = STATUS && (OCI_DPR_COMPLETE == dp->res_conv);
    RETVAL = dp->res_conv;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathLoad
 * --------------------------------------------------------------------------------------------- */

unsigned int DirPathLoad
(
    OCI_DirPath *dp
)
{
    CALL_ENTER(unsigned int, OCI_DPR_ERROR)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_CONVERTED)
    CALL_CONTEXT_FROM_CON(dp->con)

    /* reset the number of processed rows */

    dp->nb_processed = 0;

   /* reset errors variables as OCI_DirPathLoad() is not re-entrant */

    dp->nb_err       = 0;
    dp->idx_err_col  = 0;
    dp->idx_err_row  = 0;
    dp->res_load     = OCI_DPR_COMPLETE;

    /* load the stream */

    dp->res_load = DirPathLoadStream(dp);

    /* continue to load the stream while it returns an error */

    while (OCI_DPR_ERROR == dp->res_load)
    {
        dp->res_load = DirPathLoadStream(dp);
    }

    STATUS = STATUS && (OCI_DPR_COMPLETE == dp->res_load);
    RETVAL = dp->res_load;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathFinish
 * --------------------------------------------------------------------------------------------- */

boolean DirPathFinish
(
    OCI_DirPath *dp
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)
    CALL_CONTEXT_FROM_CON(dp->con)

    EXEC(OCIDirPathFinish(dp->ctx, dp->con->err))

    if (STATUS)
    {
        dp->status = OCI_DPS_TERMINATED;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathAbort
 * --------------------------------------------------------------------------------------------- */

boolean DirPathAbort
(
    OCI_DirPath *dp
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)
    CALL_CONTEXT_FROM_CON(dp->con)

    EXEC(OCIDirPathAbort(dp->ctx, dp->con->err))

    if (STATUS)
    {
        dp->status = OCI_DPS_TERMINATED;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathSave
 * --------------------------------------------------------------------------------------------- */

boolean DirPathSave
(
    OCI_DirPath *dp
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)
    CALL_CONTEXT_FROM_CON(dp->con)

    EXEC(OCIDirPathDataSave(dp->ctx, dp->con->err, OCI_DIRPATH_DATASAVE_SAVEONLY))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathFlushRow
 * --------------------------------------------------------------------------------------------- */

boolean DirPathFlushRow
(
    OCI_DirPath *dp
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)
    CALL_CONTEXT_FROM_CON(dp->con)

    EXEC(OCIDirPathFlushRow(dp->ctx, dp->con->err))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathSetCurrentRows
 * --------------------------------------------------------------------------------------------- */

boolean DirPathSetCurrentRows
(
    OCI_DirPath *dp,
    unsigned int nb_rows
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)
    CALL_CHECK_BOUND(dp->con, nb_rows, 1, dp->nb_rows)
    CALL_CONTEXT_FROM_CON(dp->con)

    dp->nb_cur = (ub2) nb_rows;

    RETVAL = TRUE;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathGetCurrentRows
 * --------------------------------------------------------------------------------------------- */

unsigned int DirPathGetCurrentRows
(
    OCI_DirPath *dp
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_DIRPATH, dp, nb_cur, dp->con, NULL, dp->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathGetMaxRows
 * --------------------------------------------------------------------------------------------- */

unsigned int DirPathGetMaxRows
(
    OCI_DirPath *dp
)
{
   GET_PROP(unsigned int, 0, OCI_IPC_DIRPATH, dp, nb_rows, dp->con, NULL, dp->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathSetDateFormat
 * --------------------------------------------------------------------------------------------- */

boolean DirPathSetDateFormat
(
    OCI_DirPath *dp,
    const otext *format
)
{
    dbtext  *dbstr = NULL;
    int     dbsize = -1;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    CALL_CONTEXT_FROM_CON(dp->con)

    dbsize = -1;
    dbstr  = StringGetDBString(format, &dbsize);

    ATTRIB_SET(OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_DATEFORMAT, dp->ctx, dbstr, dbsize)

    StringReleaseDBString(dbstr);

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathSetParallel
 * --------------------------------------------------------------------------------------------- */

boolean DirPathSetParallel
(
    OCI_DirPath *dp,
    boolean      value
)
{
    ub1 enabled = (ub1) value;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    CALL_CONTEXT_FROM_CON(dp->con)

    ATTRIB_SET(OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_DIRPATH_PARALLEL, dp->ctx, &enabled, sizeof(enabled))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathSetNoLog
 * --------------------------------------------------------------------------------------------- */

boolean DirPathSetNoLog
(
    OCI_DirPath *dp,
    boolean      value
)
{
    ub1 nolog = (ub1) value;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    CALL_CONTEXT_FROM_CON(dp->con)

    ATTRIB_SET(OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_DIRPATH_NOLOG, dp->ctx, &nolog, sizeof(nolog))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathSetCacheSize
 * --------------------------------------------------------------------------------------------- */

boolean DirPathSetCacheSize
(
    OCI_DirPath *dp,
    unsigned int size
)
{
    ub4 cache_size  = size;
    boolean enabled = FALSE;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    CALL_CONTEXT_FROM_CON(dp->con)

#if OCI_VERSION_COMPILE >= OCI_9_2

    ATTRIB_SET(OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_DIRPATH_DCACHE_SIZE, dp->ctx, &cache_size, sizeof(cache_size))
    ATTRIB_SET(OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_DIRPATH_DCACHE_DISABLE, dp->ctx, &enabled, sizeof(enabled))

#else

    OCI_NOT_USED(cache_size)
    OCI_NOT_USED(enabled)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathSetBufferSize
 * --------------------------------------------------------------------------------------------- */

boolean DirPathSetBufferSize
(
    OCI_DirPath *dp,
    unsigned int size
)
{
    ub4 bufsize = (ub4) size;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    CALL_CONTEXT_FROM_CON(dp->con)

    ATTRIB_SET(OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_BUF_SIZE, dp->ctx, &bufsize, sizeof(bufsize))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathSetConvertMode
 * --------------------------------------------------------------------------------------------- */

boolean DirPathSetConvertMode
(
    OCI_DirPath *dp,
    unsigned int mode
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    CALL_CHECK_ENUM_VALUE(dp->con, NULL, mode, ConversionModeValues, OTEXT("Conversion mode"))
    CALL_CONTEXT_FROM_CON(dp->con)

    dp->cvt_mode = (ub2)mode;

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathGetRowCount
 * --------------------------------------------------------------------------------------------- */

unsigned int DirPathGetRowCount
(
    OCI_DirPath *dp
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_DIRPATH, dp, nb_loaded, dp->con, NULL, dp->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathGetAffectedRows
 * --------------------------------------------------------------------------------------------- */

unsigned int DirPathGetAffectedRows
(
    OCI_DirPath *dp
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_DIRPATH, dp, nb_processed, dp->con, NULL, dp->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathGetErrorColumn
 * --------------------------------------------------------------------------------------------- */

unsigned int DirPathGetErrorColumn
(
    OCI_DirPath *dp
)
{
    CALL_ENTER(unsigned int, 0)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CONTEXT_FROM_CON(dp->con)

    if (dp->idx_err_col < dp->nb_err)
    {
        RETVAL = (unsigned int) dp->err_cols[dp->idx_err_col++] + 1;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPathGetErrorRow
 * --------------------------------------------------------------------------------------------- */

unsigned int DirPathGetErrorRow
(
    OCI_DirPath *dp
)
{
    CALL_ENTER(unsigned int, 0)
    CALL_CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CALL_CONTEXT_FROM_CON(dp->con)

    if (dp->idx_err_row < dp->nb_err)
    {
        RETVAL = (unsigned int) dp->err_rows[dp->idx_err_row++] + 1;
    }

    CALL_EXIT()
}
