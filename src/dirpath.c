/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2024 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "connection.h"
#include "macros.h"
#include "memory.h"
#include "number.h"
#include "stringutils.h"

static const unsigned int ConversionModeValues[] =
{
    OCI_DCM_DEFAULT,
    OCI_DCM_FORCE
};

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathSetArray
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibDirPathSetArray
(
    OCI_DirPath *dp,
    ub4          row_from
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    ub1 *data = NULL;
    ub4 size = 0;
    ub1 flag = 0;
    ub2 col  = 0;
    ub4 row  = 0;

    CHECK_PTR(OCI_IPC_DIRPATH, dp)

    /* reset the number of entries et */

    dp->nb_entries = 0;

    /* set entries */

    for (row = row_from; row < dp->nb_cur; row++)
    {
        for (col = 0; col < dp->nb_cols; col++)
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

            CHECK_OCI
            (
                dp->con->err,
                OCIDirPathColArrayEntrySet,
                dp->arr, dp->con->err, (ub4) dp->nb_entries,
                (ub2) (col), (ub1*) data, (ub4) size, flag
            )
        }

        /* increment number of item set */
        dp->nb_entries++;
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPahArrayToStream
 * --------------------------------------------------------------------------------------------- */

static unsigned int OcilibDirPathArrayToStream
(
    OCI_DirPath *dp,
    ub4          row_from
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_DPR_ERROR,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    sword ret = OCI_SUCCESS;

    unsigned int status = OCI_DPR_ERROR;

    CHECK_PTR(OCI_IPC_DIRPATH, dp)

    /* convert the array to a stream */

    ret = OCIDirPathColArrayToStream(dp->arr, dp->ctx, dp->strm, dp->con->err, dp->nb_entries, (ub4) 0);

    switch (ret)
    {
        case OCI_SUCCESS:
        {
            status     = OCI_DPR_COMPLETE;
            dp->status = OCI_DPS_CONVERTED;
            break;
        }
        case OCI_ERROR:
        {
            status = OCI_DPR_ERROR;

            /* only raise the exception if we're not in force mode */

            if (OCI_DCM_DEFAULT == dp->cvt_mode)
            {
                OcilibExceptionOCI(&call_context, dp->con->err, ret);
            }
            break;
        }
        case OCI_CONTINUE:
        {
            status     = OCI_DPR_FULL;
            dp->status = OCI_DPS_CONVERTED;
            break;
        }
        case OCI_NEED_DATA:
        {
            status = OCI_DPR_PARTIAL;
            break;
        }
    }

    if (OCI_FAILURE(ret))
    {
        ub4 err_row = 0;
        ub2 err_col = 0;
        ub4 size    = 0;

        size = sizeof(err_col);

        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_DIRPATH_COLUMN_ARRAY, OCI_ATTR_COL_COUNT,
            dp->arr, &err_col, &size,
            dp->con->err
        )

        size = sizeof(err_row);

        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_DIRPATH_COLUMN_ARRAY, OCI_ATTR_ROW_COUNT,
            dp->arr, &err_row, &size,
            dp->con->err
        )

        /* update converted rows so far */
        dp->nb_converted += err_row;

        /* record errors index on real error */
        if (OCI_DPR_ERROR == status)
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

    SET_RETVAL(status)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * DirPahArrayToStream
 * --------------------------------------------------------------------------------------------- */

static unsigned int OcilibDirPathLoadStream
(
    OCI_DirPath *dp
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_DPR_ERROR,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    sword ret = OCI_SUCCESS;

    ub4 nb_loaded   = 0;
    ub4 size_loaded = sizeof(nb_loaded);
    ub4 size_offset = sizeof(ub4);

    CHECK_PTR(OCI_IPC_DIRPATH, dp)

    unsigned int status = OCI_DPR_ERROR;

    /* load the stream */

    ret = OCIDirPathLoadStream(dp->ctx, dp->strm, dp->con->err);

    switch (ret)
    {
        case OCI_SUCCESS:
        {
            status     = OCI_DPR_COMPLETE;
            dp->status = OCI_DPS_PREPARED;
            break;
        }
        case OCI_ERROR:
        {
            status = OCI_DPR_ERROR;
            OcilibExceptionOCI(&call_context, dp->con->err, ret);
            break;
        }
        case OCI_NO_DATA:
        {
            status = OCI_DPR_EMPTY;
            break;
        }
        case OCI_NEED_DATA:
        {
            status = OCI_DPR_PARTIAL;
            break;
        }
    }

    /* retrieve the number of rows loaded so far */

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_DIRPATH_STREAM, OCI_ATTR_ROW_COUNT,
        dp->strm, &nb_loaded, &size_loaded,
        dp->con->err
    )

    /* retrieve the offset of the last processed row */

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_DIRPATH_STREAM, OCI_ATTR_STREAM_OFFSET,
        dp->strm, &dp->load_offset, &size_offset,
        dp->con->err
    )

    dp->nb_loaded    += nb_loaded;
    dp->nb_processed += nb_loaded;

    /* On failure, record errors rows */

    if (OCI_FAILURE(ret))
    {
        dp->err_rows[dp->nb_err] = (dp->nb_err > 0) ? (dp->err_rows[dp->nb_err-1] + nb_loaded + 1) : dp->nb_loaded;
        dp->err_cols[dp->nb_err] = 0;
        dp->nb_err++;
    }

    SET_RETVAL(status)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathCreate
 * --------------------------------------------------------------------------------------------- */

OCI_DirPath * OcilibDirPathCreate
(
    OCI_TypeInfo *typinf,
    const otext  *partition,
    unsigned int  nb_cols,
    unsigned int  nb_rows
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_DirPath*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    )

    dbtext* dbstr1 = NULL;
    dbtext* dbstr2 = NULL;
    dbtext* dbstr3 = NULL;

    int dbsize1 = -1;
    int dbsize2 = -1;
    int dbsize3 = -1;

    OCI_DirPath* dp = NULL;

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CHECK_COMPAT(typinf->type != OCI_TIF_TYPE)
    CHECK_BOUND(nb_cols, 1, typinf->nb_cols)

    /* allocate direct path structure */

    ALLOC_DATA(OCI_IPC_DIRPATH, dp, 1)

    dp->con      = typinf->con;
    dp->status   = OCI_DPS_NOT_PREPARED;
    dp->cvt_mode = OCI_DCM_DEFAULT;
    dp->res_conv = OCI_DPR_EMPTY;
    dp->res_load = OCI_DPR_EMPTY;
    dp->typinf   = typinf;
    dp->nb_rows  = (ub2)nb_rows;
    dp->nb_cols  = (ub2)nb_cols;
    dp->nb_cur   = (ub2)dp->nb_rows;

    /* allocates direct context handle */

    CHECK
    (
        OcilibMemoryAllocHandle
        (
            (dvoid *) dp->con->env,
            (dvoid **) (void *) &dp->ctx,
            OCI_HTYPE_DIRPATH_CTX
        )
    )

    /* set table name attribute */

    dbstr1 = OcilibStringGetDBString(dp->typinf->name, &dbsize1);

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_NAME,
        dp->ctx, dbstr1, dbsize1,
        typinf->con->err
    )

    /* set schema name attribute */

    if (IS_STRING_VALID(dp->typinf->schema))
    {
        dbstr2 = OcilibStringGetDBString(dp->typinf->schema, &dbsize2);

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_SCHEMA_NAME,
            dp->ctx, dbstr2, dbsize2,
            typinf->con->err
        )
    }

    /* set partition name attribute */

    if (IS_STRING_VALID(partition))
    {
        dbstr3 = OcilibStringGetDBString(partition, &dbsize3);

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_SUB_NAME,
            dp->ctx, dbstr3, dbsize3,
            typinf->con->err
        )
    }

    if (Env.version_runtime >= OCI_9_0)
    {
        ub4 num_rows = dp->nb_rows;

        /* set array size attribute */

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_NUM_ROWS,
            dp->ctx, &num_rows, sizeof(num_rows),
            typinf->con->err
        )
    }

    /* set columns count attribute */

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_NUM_COLS,
        dp->ctx, &dp->nb_cols, sizeof(dp->nb_cols),
        typinf->con->err
    )

    /* allocating the column array */

    ALLOC_DATA(OCI_IPC_DP_COL_ARRAY, dp->cols, dp->nb_cols)

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr1);
        OcilibStringReleaseDBString(dbstr2);
        OcilibStringReleaseDBString(dbstr3);

        if (FAILURE)
        {
            OcilibDirPathFree(dp);
            dp = NULL;
        }

        SET_RETVAL(dp)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathFree
(
    OCI_DirPath *dp
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    ub2 i = 0;

    CHECK_PTR(OCI_IPC_DIRPATH, dp)

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

    OcilibMemoryFreeHandle(dp->strm, OCI_HTYPE_DIRPATH_STREAM);
    OcilibMemoryFreeHandle(dp->arr,  OCI_HTYPE_DIRPATH_COLUMN_ARRAY);
    OcilibMemoryFreeHandle(dp->ctx,  OCI_HTYPE_DIRPATH_CTX);

    OcilibErrorResetSource(NULL, dp);

    FREE(dp)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathSetColumn
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathSetColumn
(
    OCI_DirPath *dp,
    unsigned int index,
    const otext *name,
    unsigned int maxsize,
    const otext *format
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    OCI_DirPathColumn *dpcol = NULL;
    OCI_Column *col     = NULL;
    OCIParam   *hattr   = NULL;
    OCIParam   *hlist   = NULL;
    dbtext     *dbstr1  = NULL;
    int         dbsize1 = -1;
    dbtext     *dbstr2  = NULL;
    int         dbsize2 = -1;

    ub2 i = 0;

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    CHECK_PTR(OCI_IPC_STRING, name)
    CHECK_BOUND(index, 1, dp->nb_cols)

    /* check if column exists */

    for (i = 0; i < dp->typinf->nb_cols; i++)
    {
        if (0 == OcilibStringCaseCompare(name, dp->typinf->cols[i].name))
        {
            break;
        }
    }

    /* check if column was found */

    if (i >= dp->typinf->nb_cols)
    {
        THROW(OcilibExceptionDirPathColNotFound, name, dp->typinf->name)
    }

    /* set column information */

    col   = &dp->typinf->cols[i];
    dpcol = &dp->cols[index-1];

    /* default column attributes */

    dpcol->maxsize     = (ub4) maxsize;
    dpcol->bufsize     = (ub4) maxsize + 1;
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
                dpcol->format      = OcilibStringDuplicate(format);
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
                dpcol->format      = OcilibStringDuplicate(format);
                dpcol->format_size = (ub4) ostrlen(format);
                dpcol->maxsize     = (ub4) max(dpcol->format_size, maxsize);
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
            THROW(OcilibExceptionDatatypeNotSupported, col->libcode)
        }
    }

    /* get column parameter list handle */

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_LIST_COLUMNS,
        dp->ctx, &hlist, NULL,
        dp->typinf->con->err
    )

    /* get column attribute handle */

    CHECK_OCI
    (
        dp->typinf->con->err,
        OCIParamGet,
        (dvoid *) hlist, OCI_DTYPE_PARAM,
        dp->con->err,
        (dvoid** ) (dvoid *) &hattr,
        (ub4) index
    )

    /* set column name */

    dbstr1 = OcilibStringGetDBString(name, &dbsize1);

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_PARAM, OCI_ATTR_NAME,
        hattr, dbstr1, dbsize1,
        dp->typinf->con->err
    )

    /* set column type */

    CHECK_ATTRIB_SET
    (
        OCI_DTYPE_PARAM, OCI_ATTR_DATA_TYPE,
        hattr, &dpcol->sqlcode, sizeof(dpcol->sqlcode),
        dp->typinf->con->err
    )

    /* set column size */

    if (OcilibConnectionIsVersionSupported(dp->typinf->con, OCI_10_1))
    {
        CHECK_ATTRIB_SET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_DATA_SIZE,
            hattr, &dpcol->maxsize, sizeof(dpcol->maxsize),
            dp->typinf->con->err
        )
    }
    else
    {
        ub2 legacy_col_size = (ub2) dpcol->maxsize;

        CHECK_ATTRIB_SET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_DATA_SIZE,
            hattr, &legacy_col_size, sizeof(legacy_col_size),
            dp->typinf->con->err
        )       
    }

    /* set column precision */

    if (col->prec != 0)
    {
        CHECK_ATTRIB_SET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_PRECISION,
            hattr, &col->prec, sizeof(col->prec),
            dp->typinf->con->err
        )
    }

    /* set column scale */

    if (col->scale != 0)
    {
        CHECK_ATTRIB_SET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_SCALE,
            hattr, &col->scale, sizeof(col->scale),
            dp->typinf->con->err
        )
    }

    /* set column date/time format attribute */

    if (dpcol->format && dpcol->format[0] && (OCI_DDT_NUMBER != dpcol->type))
    {
        dbstr2 = OcilibStringGetDBString(dpcol->format, &dbsize2);

        CHECK_ATTRIB_SET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_DATEFORMAT,
            hattr, dbstr2, dbsize2,
            dp->typinf->con->err
        )
    }

    /* setup Unicode mode for Unicode user data */

    if (OCI_DDT_TEXT == dpcol->type && OCI_CHAR_WIDE == Env.charset)
    {
        ub2 csid = OCI_UTF16ID;

        CHECK_ATTRIB_SET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_CHARSET_ID,
            hattr, &csid, sizeof(csid),
            dp->typinf->con->err
        )
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr1);
        OcilibStringReleaseDBString(dbstr2);

        if (NULL != hattr)
        {
            OCIDescriptorFree(hattr, OCI_DTYPE_PARAM);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathPrepare
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathPrepare
(
    OCI_DirPath *dp
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    ub4 num_rows = 0;
    ub4 size = sizeof(num_rows);

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)

    /* prepare direct path operation */

    CHECK_OCI
    (
        dp->typinf->con->err,
        OCIDirPathPrepare,
        dp->ctx, dp->con->cxt,
        dp->con->err
    )

    /* allocate column array handle */

    CHECK
    (
        OcilibMemoryAllocHandle
        (
            (dvoid *)dp->ctx,
            (dvoid **)(void *)&dp->arr,
            OCI_HTYPE_DIRPATH_COLUMN_ARRAY
        )
    )

    /* allocate stream handle */

    CHECK
    (
        OcilibMemoryAllocHandle
        (
            (dvoid *)dp->ctx,
            (dvoid **)(void *)&dp->strm,
            OCI_HTYPE_DIRPATH_STREAM
        )
    )

    /* check the number of rows allocated */

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_DIRPATH_COLUMN_ARRAY, OCI_ATTR_NUM_ROWS,
        dp->arr, &num_rows, &size,
        dp->typinf->con->err
    )

    dp->nb_cur  = (ub2) num_rows;
    dp->nb_rows = (ub2) num_rows;

    /* allocate array of errs rows */

    ALLOC_DATA(OCI_IPC_BUFF_ARRAY, dp->err_rows, dp->nb_cur)

    /* allocate array of errs cols */

    ALLOC_DATA(OCI_IPC_BUFF_ARRAY, dp->err_cols, dp->nb_cur)

    /* now, we need to allocate internal buffers */

    for (ub2 i = 0; i < dp->nb_cols; i++)
    {
        OCI_DirPathColumn *col = &dp->cols[i];

        ALLOC_BUFFER(OCI_IPC_BUFF_ARRAY, col->data,  col->bufsize, dp->nb_cur)
        ALLOC_BUFFER(OCI_IPC_BUFF_ARRAY, col->lens,  sizeof(ub4),  dp->nb_cur)
        ALLOC_BUFFER(OCI_IPC_BUFF_ARRAY, col->flags, sizeof(ub1),  dp->nb_cur)
    }

    dp->status = OCI_DPS_PREPARED;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathSetEntry
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathSetEntry
(
    OCI_DirPath *dp,
    unsigned int row,
    unsigned int index,
    void        *value,
    unsigned int size,
    boolean      complete
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    OCI_DirPathColumn *dpcol = NULL;

    ub1 flag = 0;

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)
    CHECK_BOUND(index, 1, dp->nb_cols)
    CHECK_BOUND(row,   1, dp->nb_cur)

    dpcol = &dp->cols[index-1];
    CHECK_NULL(dpcol)

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

    if (NULL != value)
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
            OcilibStringUTF32ToUTF16(value, data, size);
        }
        else if (OCI_DDT_OTHERS == dpcol->type && OCI_CHAR_WIDE == Env.charset)
        {
            /* input Unicode numeric values causes oracle conversion error.
               so, let's convert them to ANSI */

            size = ocharcount(size);
            OcilibStringNativeToAnsi(value, data, size);
        }
        else if (OCI_DDT_NUMBER == dpcol->type)
        {
            /* if a format was provided for a numeric column, we convert the input
            buffer to a OCINumber */

            OCINumber *num = (OCINumber *) data;

            CHECK(OcilibNumberFromStringInternal(dp->con, num, OCI_NUM_NUMBER,
                                                 (otext *)value, dpcol->format))

            size = (unsigned int) num->OCINumberPart[0];
        }
        else
        {
            memcpy(data, value, (size_t) size);
        }
    }

    dpcol->lens[row-1]  = size;
    dpcol->flags[row-1] = flag;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathReset
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathReset
(
    OCI_DirPath *dp
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    CHECK_PTR(OCI_IPC_DIRPATH, dp)

    /* reset conversion and loading variables */

    dp->nb_processed = 0;
    dp->nb_converted = 0;
    dp->nb_err       = 0;
    dp->idx_err_row  = 0;
    dp->idx_err_col  = 0;
    dp->load_offset  = 0;

    /* reset array */

    CHECK_OCI
    (
        dp->typinf->con->err,
        OCIDirPathColArrayReset,
        dp->arr, dp->con->err
    )

    /* reset stream */

    CHECK_OCI
    (
        dp->typinf->con->err,
        OCIDirPathStreamReset,
        dp->strm, dp->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathConvert
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibDirPathConvert
(
    OCI_DirPath *dp
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_DPR_ERROR,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    ub4 row_from = 0;

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)

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
        CHECK_OCI
        (
            dp->typinf->con->err,
            OCIDirPathStreamReset,
            dp->strm, dp->con->err
        )
    }

    /* reset conversion status back to default error value */

    dp->res_conv = OCI_DPR_ERROR;

    /* set array values */

    CHECK(OcilibDirPathSetArray(dp, row_from))

    /* try to convert values from array into stream */

    dp->res_conv = OcilibDirPathArrayToStream(dp, row_from);

    /* in case of conversion error, continue conversion in force mode
       other return from conversion */

    if (OCI_DCM_FORCE == dp->cvt_mode && OCI_DPR_ERROR == dp->res_conv)
    {
        /* perform conversion until all non erred rows are converted */

        while (OCI_DPR_ERROR == dp->res_conv && dp->nb_err <= dp->nb_cur)
        {
            /* start from the row that follows the last erred row */

            row_from = dp->err_rows[dp->nb_err - 1] + 1;

            /* set values again */

            CHECK(OcilibDirPathSetArray(dp, row_from))

            /* perform conversion again */

            dp->res_conv = OcilibDirPathArrayToStream(dp, row_from);
        }
    }

    dp->nb_processed = dp->nb_converted;

    SET_RETVAL(dp->res_conv)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathLoad
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibDirPathLoad
(
    OCI_DirPath *dp
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_DPR_ERROR,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    ub4 load_offset = 0;

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_CONVERTED)

    /* reset the number of processed rows */

    dp->nb_processed = 0;

    /* reset errors variables as OCI_DirPathLoad() is not re-entrant */

    dp->nb_err      = 0;
    dp->idx_err_col = 0;
    dp->idx_err_row = 0;
    dp->res_load    = OCI_DPR_COMPLETE;

    /* load the stream */

    dp->res_load = OcilibDirPathLoadStream(dp);

    load_offset = dp->load_offset;

    /* continue to load the stream while it returns an error */

    while (OCI_DPR_ERROR == dp->res_load)
    {
        dp->res_load = OcilibDirPathLoadStream(dp);

        /* if number of converted rows does not increase, the error is not related to conversion error
           thus, we need to exit the loop to not iterate for ever 
        */
        if (load_offset >= dp->load_offset)
        {
            break;
        }

        load_offset = dp->load_offset;
    }

    SET_RETVAL(dp->res_load)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathFinish
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathFinish
(
    OCI_DirPath *dp
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)

    CHECK_OCI
    (
        dp->typinf->con->err,
        OCIDirPathFinish,
        dp->ctx, dp->con->err
    )

    dp->status = OCI_DPS_TERMINATED;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathAbort
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathAbort
(
    OCI_DirPath *dp
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)

    CHECK_OCI
    (
        dp->typinf->con->err,
        OCIDirPathAbort,
        dp->ctx, dp->con->err
    )

    dp->status = OCI_DPS_TERMINATED;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathSave
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathSave
(
    OCI_DirPath *dp
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)

    CHECK_OCI
    (
        dp->typinf->con->err,
        OCIDirPathDataSave,
        dp->ctx, dp->con->err,
        OCI_DIRPATH_DATASAVE_SAVEONLY
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathFlushRow
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathFlushRow
(
    OCI_DirPath *dp
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)

    CHECK_OCI
    (
        dp->typinf->con->err,
        OCIDirPathFlushRow,
        dp->ctx, dp->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathSetCurrentRows
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathSetCurrentRows
(
    OCI_DirPath *dp,
    unsigned int nb_rows
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_PREPARED)
    CHECK_BOUND(nb_rows, 1, dp->nb_rows)

    dp->nb_cur = (ub2) nb_rows;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathGetCurrentRows
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibDirPathGetCurrentRows
(
    OCI_DirPath *dp
)
{
    GET_PROP
    (
        /* result */ unsigned int, 0,
        /* handle */ OCI_IPC_DIRPATH, dp,
        /* member */ nb_cur
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathGetMaxRows
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibDirPathGetMaxRows
(
    OCI_DirPath *dp
)
{
    GET_PROP
    (
        /* result */ unsigned int, 0,
        /* handle */ OCI_IPC_DIRPATH, dp,
        /* member */ nb_rows
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathSetDateFormat
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathSetDateFormat
(
    OCI_DirPath *dp,
    const otext *format
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    dbtext *dbstr = NULL;
    int dbsize = -1;

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)

    dbstr = OcilibStringGetDBString(format, &dbsize);

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_DATEFORMAT,
        dp->ctx, dbstr, dbsize,
        dp->typinf->con->err
    )

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathSetParallel
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathSetParallel
(
    OCI_DirPath *dp,
    boolean      value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    ub1 enabled = (ub1) value;

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_DIRPATH_PARALLEL,
        dp->ctx, &enabled, sizeof(enabled),
        dp->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathSetNoLog
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathSetNoLog
(
    OCI_DirPath *dp,
    boolean      value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    ub1 nolog = (ub1) value;

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_DIRPATH_NOLOG,
        dp->ctx, &nolog, sizeof(nolog),
        dp->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathSetCacheSize
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathSetCacheSize
(
    OCI_DirPath *dp,
    unsigned int size
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    ub4 cache_size = size;
    boolean enabled = FALSE;

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)

#if OCI_VERSION_COMPILE >= OCI_9_2

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_DIRPATH_DCACHE_SIZE,
        dp->ctx, &cache_size, sizeof(cache_size),
        dp->typinf->con->err
    )

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_DIRPATH_DCACHE_DISABLE,
        dp->ctx, &enabled, sizeof(enabled),
        dp->typinf->con->err
    )

#else

    OCI_NOT_USED(cache_size)
    OCI_NOT_USED(enabled)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathSetBufferSize
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathSetBufferSize
(
    OCI_DirPath *dp,
    unsigned int size
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    ub4 bufsize = (ub4) size;

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)

    CHECK_ATTRIB_SET
    (
        OCI_HTYPE_DIRPATH_CTX, OCI_ATTR_BUF_SIZE,
        dp->ctx, &bufsize, sizeof(bufsize),
        dp->typinf->con->err
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathSetConvertMode
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDirPathSetConvertMode
(
    OCI_DirPath *dp,
    unsigned int mode
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    CHECK_PTR(OCI_IPC_DIRPATH, dp)
    CHECK_DIRPATH_STATUS(dp, OCI_DPS_NOT_PREPARED)
    CHECK_ENUM_VALUE(mode, ConversionModeValues, OTEXT("Conversion mode"))

    dp->cvt_mode = (ub2)mode;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathGetRowCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibDirPathGetRowCount
(
    OCI_DirPath *dp
)
{
    GET_PROP
    (
        /* result */ unsigned int, 0,
        /* handle */ OCI_IPC_DIRPATH, dp,
        /* member */ nb_loaded
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathGetAffectedRows
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibDirPathGetAffectedRows
(
    OCI_DirPath *dp
)
{
    GET_PROP
    (
        /* result */ unsigned int, 0,
        /* handle */ OCI_IPC_DIRPATH, dp,
        /* member */ nb_processed
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathGetErrorColumn
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibDirPathGetErrorColumn
(
    OCI_DirPath *dp
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    CHECK_PTR(OCI_IPC_DIRPATH, dp)

    CHECK(dp->idx_err_col < dp->nb_err)

    SET_RETVAL((unsigned int) dp->err_cols[dp->idx_err_col++] + 1)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDirPathGetErrorRow
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibDirPathGetErrorRow
(
    OCI_DirPath *dp
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_DIRPATH, dp
    )

    CHECK_PTR(OCI_IPC_DIRPATH, dp)

    CHECK(dp->idx_err_row < dp->nb_err)

    SET_RETVAL((unsigned int) dp->err_rows[dp->idx_err_row++] + 1)

    EXIT_FUNC()
}
