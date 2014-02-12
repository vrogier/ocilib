/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2014 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: column.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnDescribe
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ColumnDescribe
(
    OCI_Column     *col,
    OCI_Connection *con,
    OCI_Statement  *stmt,
    void           *handle,
    int             index,
    int             ptype
)
{
    void    *param    = NULL;
    boolean  res      = TRUE;

    /* get descriptor */

    if (OCI_DESC_COLLECTION == ptype)
    {
        OCI_CALL1
        (
            res, con, stmt,

            OCIAttrGet((dvoid *) handle, (ub4) OCI_DTYPE_PARAM, (dvoid *) &param,
                       (ub4 *) NULL, (ub4) OCI_ATTR_COLLECTION_ELEMENT, con->err)
        )
    }
    else
    {
        ub4 htype = (OCI_DESC_RESULTSET == ptype) ? OCI_HTYPE_STMT : OCI_DTYPE_PARAM;

        OCI_CALL1
        (
            res, con, stmt,

            OCIParamGet((dvoid *) handle, htype,  con->err, (void**) &param, (ub4) index)
        )
    }

    /* sql code */

    OCI_CALL1
    (
        res, con, stmt,

        OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM, (dvoid *) &col->sqlcode,
                   (ub4 *) NULL, (ub4) OCI_ATTR_DATA_TYPE, con->err)
    )

    /* size */

    OCI_CALL1
    (
        res, con, stmt,

        OCIAttrGet((dvoid *) param, (ub4)  OCI_DTYPE_PARAM, (dvoid *) &col->size,
                   (ub4 *) NULL, (ub4) OCI_ATTR_DATA_SIZE, con->err)
    )

    /* scale */

    OCI_CALL1
    (
        res, con, stmt,

        OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM, (dvoid *) &col->scale,
                   (ub4 *) NULL, (ub4) OCI_ATTR_SCALE, con->err)
    )

    /* precision */

    if (OCI_DESC_RESULTSET == ptype)
    {
        sb2 prec = 0;

        OCI_CALL1
        (
            res, con, stmt,

            OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM, (dvoid *) &prec,
                       (ub4 *) NULL, (ub4) OCI_ATTR_PRECISION, con->err)
        )

        col->prec = (sb2) prec;
    }
    else
    {
        ub1 prec = 0;

        OCI_CALL1
        (
            res, con, stmt,

            OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM, (dvoid *) &prec,
                       (ub4 *) NULL, (ub4) OCI_ATTR_PRECISION, con->err)
        )

        col->prec = (sb2) prec;
    }

    /* charset form */

    OCI_CALL1
    (
        res, con, stmt,

        OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM, (dvoid *) &col->csfrm,
                   (ub4 *) NULL, (ub4) OCI_ATTR_CHARSET_FORM, con->err)
    )

    /* type of column length for string based column */

#if OCI_VERSION_COMPILE >= OCI_9_2

    if ((OCILib.version_runtime >= OCI_9_2) && (con->ver_num >= OCI_9_2))
    {
        /* char used - no error checking because on Oracle 9.0, querying
                       this param that is not char/varchar based will cause an
                       error */

        OCI_CALL1
        (
            res, con, stmt,

            OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) &col->charused, (ub4 *) NULL,
                       (ub4) OCI_ATTR_CHAR_USED, con->err)
        )
    }

    /* char size */

    if (col->charused)
    {
        OCI_CALL1
        (
            res, con, stmt,

            OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) &col->charsize, (ub4 *) NULL,
                       (ub4) OCI_ATTR_CHAR_SIZE, con->err)
        )
    }

    if ((OCILib.version_runtime >= OCI_9_0) && (con->ver_num >= OCI_9_0))
    {
        /* fractional time precision for timestamps */

        if ( SQLT_TIMESTAMP     == col->sqlcode ||
             SQLT_TIMESTAMP_TZ  == col->sqlcode ||
             SQLT_TIMESTAMP_LTZ == col->sqlcode )
        {
            OCI_CALL1
            (
                res, con, stmt,

                OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM,
                           (dvoid *) &col->prec, (ub4 *) NULL,
                           (ub4) OCI_ATTR_FSPRECISION, con->err)
            )
        }

        /* leading and fractional precision for interval */

        if (SQLT_INTERVAL_DS == col->sqlcode  || SQLT_INTERVAL_YM == col->sqlcode)
        {
            OCI_CALL1
            (
                res, con, stmt,

                OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM,
                           (dvoid *) &col->prec, (ub4 *) NULL,
                           (ub4) OCI_ATTR_LFPRECISION, con->err)
            )

            OCI_CALL1
            (
                res, con, stmt,

                OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM,
                           (dvoid *) &col->prec2, (ub4 *) NULL,
                           (ub4) OCI_ATTR_FSPRECISION, con->err)
            )
        }
    }

#endif

    /* check nullable only for table based column */

    if (ptype < OCI_DESC_TYPE)
    {
        OCI_CALL1
        (
            res, con, stmt,

            OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) &col->nullable, (ub4 *) NULL,
                       (ub4) OCI_ATTR_IS_NULL, con->err)
        )
    }
    else
    {
        col->nullable = TRUE;
    }

    /* name */

    if (res == TRUE)
    {
        dbtext *dbstr    = NULL;
        int     dbsize   = 0;
        ub4     attrname = (OCI_DESC_COLLECTION == ptype) ? OCI_ATTR_TYPE_NAME : OCI_ATTR_NAME;

        OCI_CALL1
        (
            res, con, stmt,

            OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM, (dvoid *) &dbstr,
                       (ub4 *) &dbsize, (ub4) attrname, con->err)
        )

        if (res && dbstr)
        {
            col->name = OCI_StringDuplicateFromOracleString(dbstr, dbcharcount(dbsize));

            res = (col->name != NULL);
        }
    }

    /* user type descriptor */

    if (SQLT_NTY == col->sqlcode || SQLT_REF == col->sqlcode)
    {
        dbtext *dbstr_name   = NULL;
        dbtext *dbstr_schema = NULL;
        int    dbsize_name   = 0;
        int    dbsize_schema = 0;

        OCI_CALL1
        (
            res, con, stmt,

            OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) &dbstr_name, (ub4 *) &dbsize_name,
                       (ub4) OCI_ATTR_TYPE_NAME, con->err)
        )

        OCI_CALL1
        (
            res, con, stmt,

            OCIAttrGet((dvoid *) param, (ub4) OCI_DTYPE_PARAM,
                       (dvoid *) &dbstr_schema, (ub4 *) &dbsize_schema,
                       (ub4) OCI_ATTR_SCHEMA_NAME, con->err)
        )

        if (res)
        {
            otext type_name[(OCI_SIZE_OBJ_NAME * 2) + 2] = OTEXT("");
  
            if (dbstr_schema && (dbsize_schema > 0))
            {
                OCI_StringOracleToNative(dbstr_schema, type_name, dbcharcount(dbsize_schema));

                ostrcat(type_name, OTEXT("."));
            }

            OCI_StringOracleToNative(dbstr_name, (((char *) type_name) + otextsize(type_name)), dbcharcount(dbsize_name));

            col->typinf = OCI_TypeInfoGet(con, type_name, OCI_TIF_TYPE);

            res = (col->typinf != NULL);
        }
    }

    if (param)
    {
        res = (OCI_SUCCESS == OCIDescriptorFree(param, OCI_DTYPE_PARAM));
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnMap
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ColumnMap
(
    OCI_Column    *col,
    OCI_Statement *stmt
)
{
    ub2 char_size = (ub2) ( (OCILib.nls_utf8 ) ? UTF8_BYTES_PER_CHAR : sizeof(otext) );

    boolean res = TRUE;

    OCI_CHECK(col == NULL, FALSE);

    /* map Oracle SQL code to OCILIB types and setup of internal buffer size */

    col->libcode = col->sqlcode;

    switch (col->libcode)
    {
        case SQLT_INT:
        {
            col->datatype = OCI_CDT_NUMERIC;

            /* set bufsize only if it's not a "returning into" placeholder */

            if (col->bufsize == 0)
            {
                col->subtype = OCI_NUM_INT;
                col->bufsize = sizeof(int);
            }

            break;
        }
        case SQLT_UIN:
        {
            col->datatype = OCI_CDT_NUMERIC;

            /* set bufsize only if it's not a "returning into" placeholder */

            if (col->bufsize == 0)
            {
                col->subtype = OCI_NUM_UINT;
                col->bufsize = sizeof(unsigned int);
            }

            break;
        }
        case SQLT_FLT:
        case SQLT_VNU:
        case SQLT_PDN:
        case SQLT_NUM:
        {
            col->datatype = OCI_CDT_NUMERIC;
            col->subtype  = OCI_NUM_NUMBER;
            col->libcode  = SQLT_VNU;
            col->bufsize  = sizeof(OCINumber);

            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_10_1

        case SQLT_BFLOAT:
        case SQLT_IBFLOAT:
        {
            col->datatype = OCI_CDT_NUMERIC;
            col->subtype  = OCI_NUM_FLOAT;
            col->libcode  = SQLT_BFLOAT;
            col->bufsize  = sizeof(float);

            break;
        }

    #endif

    #if OCI_VERSION_COMPILE >= OCI_10_1

        case SQLT_BDOUBLE:
        case SQLT_IBDOUBLE:
        {
        
            col->datatype = OCI_CDT_NUMERIC;
            col->subtype  = OCI_NUM_DOUBLE;
            col->libcode  = SQLT_BDOUBLE;
            col->bufsize  = sizeof(double);

            break;
        }

    #endif

        case SQLT_DAT:
        case SQLT_ODT:
        {
            col->datatype = OCI_CDT_DATETIME;

            /* We map to SQLT_ODT only it the column is not part of a
               "returning into" clause (workaround for Oracle
               known bug #3269146
            */

            if (col->bufsize == 0)
            {
                col->libcode = SQLT_ODT;
                col->bufsize = sizeof(OCIDate);
            }

            break;
        }
        case SQLT_CUR:
        case SQLT_RSET:
        {
            col->datatype   = OCI_CDT_CURSOR;
            col->bufsize    = sizeof(OCIStmt *);
            col->handletype = OCI_HTYPE_STMT;

            break;
        }
        case SQLT_RID:
        case SQLT_RDD:
        {
            col->libcode  = SQLT_STR;
            col->datatype = OCI_CDT_TEXT;

            if ((SQLT_RDD == col->sqlcode) || (col->size > sizeof(OCIRowid *)))
            {
                /* For Oracle 7 ROWIDs and regular ROWID descriptors, the
                   max size of the hex value is defined by the constant
                   OCI_SIZE_ROWID
                */

                col->bufsize = (ub4) ((OCI_SIZE_ROWID + 1) * char_size);
            }
            else
            {
                /*  For ROWID descriptor, if column size is bigger than the size
                    of the descriptor, it means that an UROWID column and then
                    the column size is the maximum size needed for representing
                    its value as an hex string
                */

                col->bufsize = (ub4) ((col->size + 1) * char_size);
            }

            break;
        }
        case SQLT_BIN:
        {
            /* adding one extra character space for string conversion */

            col->datatype = OCI_CDT_RAW;
            col->bufsize  = (ub4) (col->size + (ub2) sizeof(otext));

            break;
        }
        case SQLT_BLOB:
        {
            col->datatype   = OCI_CDT_LOB;
            col->subtype    = OCI_BLOB;
            col->handletype = OCI_DTYPE_LOB;
            col->bufsize    = (ub4) sizeof(OCILobLocator *);

            break;
        }
        case SQLT_CLOB:
        {
            col->datatype   = OCI_CDT_LOB;
            col->handletype = OCI_DTYPE_LOB;
            col->bufsize    = (ub4) sizeof(OCILobLocator *);

            if (SQLCS_NCHAR == col->csfrm)
            {
                col->subtype = OCI_NCLOB;
            }
            else
            {
                col->subtype = OCI_CLOB;
            }

            break;
        }
        case SQLT_BFILE:
        {
            col->datatype   = OCI_CDT_FILE;
            col->subtype    = OCI_BFILE;
            col->handletype = OCI_DTYPE_LOB;
            col->bufsize    = (ub4) sizeof(OCILobLocator *);

            break;
        }
        case SQLT_CFILE:
        {
            col->datatype   = OCI_CDT_FILE;
            col->subtype    = OCI_CFILE;
            col->bufsize    = (ub4) sizeof(OCILobLocator *);
            col->handletype = OCI_DTYPE_LOB;

            break;
        }
        case SQLT_LNG:
        case SQLT_LVC:
        case SQLT_LBI:
        case SQLT_LVB:
        case SQLT_VBI:
        {
            if ((SQLT_LNG ==col->libcode || SQLT_LVC == col->libcode) &&
                (stmt && OCI_LONG_IMPLICIT == stmt->long_mode))
            {
                col->datatype = OCI_CDT_TEXT;               
                col->subtype  = OCI_CLONG;
                col->bufsize  = (OCI_SIZE_LONG+1) * char_size;

            }
            else
            {
                col->datatype = OCI_CDT_LONG;
                col->bufsize  = INT_MAX;

                if (SQLT_LBI == col->libcode ||
                    SQLT_LVB == col->libcode ||
                    SQLT_VBI == col->libcode )
                {
                    col->subtype = OCI_BLONG;
                }
                else
                {
                    col->subtype = OCI_CLONG;
                }

            }

            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_TIMESTAMP:
        {
            col->datatype   = OCI_CDT_TIMESTAMP;
            col->subtype    = OCI_TIMESTAMP;
            col->handletype = OCI_DTYPE_TIMESTAMP;
            col->bufsize    = (ub4) sizeof(OCIDateTime *);

            break;
        }
        case SQLT_TIMESTAMP_TZ:
        {
            col->datatype   = OCI_CDT_TIMESTAMP;
            col->subtype    = OCI_TIMESTAMP_TZ;
            col->handletype = OCI_DTYPE_TIMESTAMP_TZ;
            col->bufsize    = (ub4) sizeof(OCIDateTime *);

            break;
        }
        case SQLT_TIMESTAMP_LTZ:
        {
            col->datatype   = OCI_CDT_TIMESTAMP;
            col->subtype    = OCI_TIMESTAMP_LTZ;
            col->handletype = OCI_DTYPE_TIMESTAMP_LTZ;
            col->bufsize    = (ub4) sizeof(OCIDateTime *);

            break;
        }
        case SQLT_INTERVAL_YM:
        {
            col->datatype   = OCI_CDT_INTERVAL;
            col->subtype    = OCI_INTERVAL_YM;
            col->handletype = OCI_DTYPE_INTERVAL_YM;
            col->bufsize    = (ub4) sizeof(OCIInterval *);

            break;
        }
        case SQLT_INTERVAL_DS:
        {
            col->datatype   = OCI_CDT_INTERVAL;
            col->subtype    = OCI_INTERVAL_DS;
            col->handletype = OCI_DTYPE_INTERVAL_DS;
            col->bufsize    = (ub4) sizeof(OCIInterval *);

            break;
        }
            
    #endif

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_PNTY:

    #endif

        case SQLT_NTY:
        {
            col->libcode  = SQLT_NTY;
            col->bufsize  = (ub4) sizeof(void *);
            col->datatype = (SQLT_NCO == col->typinf->typecode) ? OCI_CDT_COLLECTION : OCI_CDT_OBJECT;

            break;
        }
        case SQLT_REF:
        {
            col->libcode  = SQLT_REF;
            col->bufsize  = (ub4) sizeof(OCIRef *);
            col->datatype = OCI_CDT_REF;

            break;
        }
        case SQLT_CHR:
        case SQLT_STR:
        case SQLT_VCS:
        case SQLT_AFC:
        case SQLT_AVC:
        case SQLT_VST:
        case SQLT_LAB:
        case SQLT_OSL:
        case SQLT_SLS:
        default:
        {
            col->libcode  = SQLT_STR;
            col->datatype = OCI_CDT_TEXT;
            col->bufsize  = (ub4) ((col->size + 1) * char_size);

            break;
        }
    }

    return res;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_ColumnGetName
(
    OCI_Column *col
)
{
    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, NULL);

    return col->name;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ColumnGetType
(
    OCI_Column *col
)
{
    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return col->datatype;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetCharsetForm
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ColumnGetCharsetForm
(
    OCI_Column *col
)
{
    unsigned int result = OCI_CSF_NONE;

    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, OCI_CSF_NONE);

    OCI_RESULT(TRUE);

    if (SQLCS_NCHAR == col->csfrm)
    {
        result = OCI_CSF_NATIONAL;
    }
    else if (SQLCS_IMPLICIT == col->csfrm)
    {
        result = OCI_CSF_DEFAULT;
    }
    
    return result;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ColumnGetSize
(
    OCI_Column *col
)
{
    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, 0);

    OCI_RESULT(TRUE);

    /* Oracle 9i introduced CHAR attribute on string columns to indicate the
       size of the column is not in bytes (default) but in chars
       OCI_ColumnDescribe() already managed the Oracle compatibly
       version, so if col->charsize is zero it means :
       - the column is not a string column
       - the size is not in char
       - client does not support the OCI_ATTR_CHAR_SIZE attribute */

    return (col->charused && col->charsize > 0) ? col->charsize : col->size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetScale
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ColumnGetScale
(
    OCI_Column *col
)
{
    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, 0);

    OCI_RESULT(TRUE);

    return (int) col->scale;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetPrecision
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ColumnGetPrecision
(
    OCI_Column *col
)
{
    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, 0);

    OCI_RESULT(TRUE);

    return (OCI_CDT_NUMERIC == col->datatype) ? (int) col->prec : 0;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetFractionalPrecision
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ColumnGetFractionalPrecision
(
    OCI_Column *col
)
{
    int result = 0;

    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, 0);

    OCI_RESULT(TRUE);

    if (OCI_CDT_TIMESTAMP == col->datatype)
    {
        result = (int) col->prec;
    }
    else if (OCI_CDT_INTERVAL == col->datatype)
    {
        result = (int) col->prec2;
    }

    return result;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetLeadingPrecision
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ColumnGetLeadingPrecision
(
    OCI_Column *col
)
{
    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, 0);

    OCI_RESULT(TRUE);

    return (OCI_CDT_INTERVAL == col->datatype) ? col->prec : 0;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetNullable
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ColumnGetNullable
(
    OCI_Column *col
)
{
    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, FALSE);

    OCI_RESULT(TRUE);

    return (1 == col->nullable);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetCharUsed
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ColumnGetCharUsed
(
    OCI_Column *col
)
{
    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, FALSE);

    OCI_RESULT(TRUE);

    return (boolean) col->charused;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetSQLType
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_ColumnGetSQLType
(
    OCI_Column *col
)
{
    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, NULL);

    /* VARCHAR type will not be returned because Oracle does not make any
       difference with VARCHAR2. If a column is created with VARCHAR, it is
       internally created as VARCHAR2
    */

    OCI_RESULT(TRUE);

    switch(col->sqlcode)
    {
        case SQLT_AFC:
        {
            return (SQLCS_NCHAR == col->csfrm) ? OTEXT("NCHAR") : OTEXT("CHAR");
        }
        case SQLT_AVC:
        case SQLT_STR:
        case SQLT_CHR:
        {
             return (SQLCS_NCHAR == col->csfrm) ? OTEXT("NVARCHAR2") : OTEXT("VARCHAR2");
        }
        case SQLT_NUM:
        {
              return (-127 == col->scale && col->prec > 0) ? OTEXT("FLOAT") : OTEXT("NUMBER");
        }
        case SQLT_INT:
        {
            return OTEXT("INTEGER");
        }
        case SQLT_FLT:
        {
            return OTEXT("FLOAT");
        }

    #if OCI_VERSION_COMPILE >= OCI_10_1

        case SQLT_BFLOAT:
        case SQLT_IBFLOAT:
        {
            return OTEXT("BINARY FLOAT");
        }
        case SQLT_BDOUBLE:
        case SQLT_IBDOUBLE:
        {
            return OTEXT("BINARY DOUBLE");
        }

    #endif

        case SQLT_LNG:
        {
            return OTEXT("LONG");
        }
        case SQLT_DAT:
        case SQLT_ODT:
        case SQLT_DATE:
        {
            return OTEXT("DATE");
        }
        case SQLT_RDD:
        case SQLT_RID:
        {
            return OTEXT("ROWID");
        }
        case SQLT_BIN:
        {
            return OTEXT("RAW");
        }
        case SQLT_LBI:
        {
            return OTEXT("LONG RAW");
        }
        case SQLT_RSET:
        {
            return OTEXT("RESULTSET");
        }
        case SQLT_CUR:
        {
            return OTEXT("CURSOR");
        }
        case SQLT_CLOB:
        {
            return (col->subtype == OCI_NCLOB) ? OTEXT("NCLOB") : OTEXT("CLOB");
        }
        case SQLT_BLOB:
        {
            return OTEXT("BLOB");
        }
        case SQLT_BFILE:
        {
            return OTEXT("BINARY FILE LOB");
        }
        case SQLT_CFILE:
        {
            return OTEXT("CFILE");
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_TIMESTAMP:
        {
            return OTEXT("TIMESTAMP");
        }
        case SQLT_TIMESTAMP_TZ:
        {
            return OTEXT("TIMESTAMP WITH TIME ZONE");
        }
        case SQLT_TIMESTAMP_LTZ:
        {
            return OTEXT("TIMESTAMP WITH LOCAL TIME ZONE");
        }
        case SQLT_INTERVAL_YM:
        {
            return OTEXT("INTERVAL YEAR TO MONTH");
        }
        case SQLT_INTERVAL_DS:
        {
            return OTEXT("INTERVAL DAY TO SECOND");
        }
           
    #endif

        case SQLT_REF:
        {
            return OTEXT("REF");
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_PNTY:

    #endif

        case SQLT_NTY:
        {
            return col->typinf ? col->typinf->name : OTEXT("NAMED TYPE");
        }
        default:
        {
            /* unknown datatype ? Should not happen because all
                 datatypes are supported */

            return OTEXT("?");
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetFullSQLType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ColumnGetFullSQLType
(
    OCI_Column  *col,
    otext       *buffer,
    unsigned int len
)
{
    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, 0);
    OCI_CHECK_PTR(OCI_IPC_STRING, buffer, 0);

    OCI_RESULT(TRUE);

    buffer[0] = 0;

    /* ISO C functions are supposed to be "standard", but we still see specific
       implementations that make some usage not portable and worse not compatible.
       MS Windows is implementing string conversion characters (%s/%ls) of the
       printf/wprintf family differently from unixes !
    */

    /* This function returns the same strings as Sql*Plus DESC command */

    switch(col->sqlcode)
    {
        case SQLT_AFC:
        {
        
        #if defined(OCI_CHARSET_WIDE) && !defined(_WINDOWS)            
            len = osprintf(buffer, len, OTEXT("%lsCHAR(%i%ls)"),
        #else
            len = osprintf(buffer, len, OTEXT("%sCHAR(%i%s)"),
        #endif
                            SQLCS_NCHAR == col->csfrm ? OTEXT("N") : OTEXT(""),
                            (int) (col->charused ? col->charsize : col->size),
                            col->charused && SQLCS_NCHAR != col->csfrm ? OTEXT(" CHAR") : OTEXT(""));
            break;
        }
        case SQLT_AVC:
        case SQLT_STR:
        case SQLT_CHR:
        {
         #if defined(OCI_CHARSET_WIDE) && !defined(_WINDOWS)
            len = osprintf(buffer, len, OTEXT("%lsVARCHAR(%i%ls)"),
         #else
            len = osprintf(buffer, len, OTEXT("%sVARCHAR(%i%s)"),
         #endif
                            SQLCS_NCHAR == col->csfrm ? OTEXT("N") : OTEXT(""),
                            (int) (col->charused ? col->charsize : col->size),
                            col->charused &&  SQLCS_NCHAR != col->csfrm ? OTEXT(" CHAR") : OTEXT(""));
            break;
        }
        case SQLT_NUM:
        {
            if (-127 == col->scale && col->prec > 0)
            {
                len = osprintf(buffer, len,  OTEXT("FLOAT(%i)"), col->prec);
            }
            else if (col->scale > 0 && col->prec > 0)
            {
                len = osprintf(buffer, len,  OTEXT("NUMBER(%i,%i)"), (int) col->prec, (int) col->scale);
            }
            else if (col->prec > 0)
            {
                len = osprintf(buffer, len,  OTEXT("NUMBER(%i)"), (int) col->prec);
            }
            else
            {
                len = osprintf(buffer, len,  OTEXT("NUMBER"));
            }

            break;
        }
        case SQLT_INT:
        {
            len = osprintf(buffer, len,  OTEXT("NUMBER"));
            break;
        }
        case SQLT_FLT:
        {
            len = osprintf(buffer, len,  OTEXT("FLOAT(%i)"), (int) col->prec);
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_10_1

        case SQLT_BFLOAT:
        case SQLT_IBFLOAT:
        {
            len = osprintf(buffer, len,  OTEXT("BINARY FLOAT"));
            break;
        }
        case SQLT_BDOUBLE:
        case SQLT_IBDOUBLE:
        {
            len = osprintf(buffer, len,  OTEXT("BINARY DOUBLE"));
            break;
        }

    #endif

        case SQLT_LNG:
        {
            len = osprintf(buffer, len, OTEXT("LONG"));
            break;
        }
        case SQLT_DAT:
        case SQLT_ODT:
        case SQLT_DATE:
        {
            len = osprintf(buffer, len, OTEXT("DATE"));
            break;
        }
        case SQLT_RDD:
        case SQLT_RID:
        {
            len = osprintf(buffer, len,  OTEXT("ROWID"));
            break;
        }
        case SQLT_BIN:
        {
            len = osprintf(buffer, len, OTEXT("RAW(%i)"), (int) col->size);
            break;
        }
        case SQLT_LBI:
        {
            len = osprintf(buffer, len, OTEXT("LONG RAW(%i)"), (int) col->size);
            break;
        }
        case SQLT_RSET:
        {
            len = osprintf(buffer, len,  OTEXT("RESULTSET"));
            break;
        }
        case SQLT_CUR:
        {
            len = osprintf(buffer, len,  OTEXT("CURSOR"));
            break;
        }
        case SQLT_CLOB:
        {
            len = osprintf(buffer, len, (col->subtype == OCI_NCLOB) ? OTEXT("NCLOB") : OTEXT("CLOB"));
            break;
        }
        case SQLT_BLOB:
        {
            len = osprintf(buffer, len,  OTEXT("BLOB"));
            break;
        }
        case SQLT_BFILE:
        {
            len = osprintf(buffer, len,  OTEXT("BINARY FILE LOB"));
            break;
        }
        case SQLT_CFILE:
        {
            len = osprintf(buffer, len,  OTEXT("CFILE"));
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_TIMESTAMP:
        {
            len = osprintf(buffer, len,  OTEXT("TIMESTAMP(%i)"), (int) col->prec);
            break;
        }
        case SQLT_TIMESTAMP_TZ:
        {
            len = osprintf(buffer, len,  OTEXT("TIMESTAMP(%i) WITH TIME ZONE"), (int) col->prec);
            break;
        }
        case SQLT_TIMESTAMP_LTZ:
        {
            len = osprintf(buffer, len,  OTEXT("TIMESTAMP(%i) WITH LOCAL TIME ZONE"), (int) col->prec);
            break;
        }
        case SQLT_INTERVAL_YM:
        {
            len = osprintf(buffer, len,  OTEXT("INTERVAL(%i) YEAR TO MONTH(%i)"),
                            (int) col->prec, (int) col->prec2);
            break;
        }
        case SQLT_INTERVAL_DS:
        {
            len = osprintf(buffer, len,  OTEXT("INTERVAL(%i) DAY TO SECOND(%i)"),
                            (int) col->prec, (int) col->prec2);
            break;
        }

    #endif

        case SQLT_REF:
        {
            len = osprintf(buffer, len,  OTEXT("REF"));
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_PNTY:

    #endif

        case SQLT_NTY:
        {
            len = osprintf(buffer, len, col->typinf ? col->typinf->name : OTEXT("NAMED TYPE"));
            break;
        }
        default:
        {
            ostrncat(buffer, OTEXT("?"), (size_t) len);
            break;
        }
    }

    return len;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OCI_API OCI_ColumnGetTypeInfo
(
    OCI_Column *col
)
{
    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, NULL);

    OCI_RESULT(TRUE);

    return col->typinf;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetSubType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ColumnGetSubType
(
    OCI_Column *col
)
{
    unsigned int type = OCI_UNKNOWN;

    OCI_CHECK_PTR(OCI_IPC_COLUMN, col, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    if (OCI_CDT_LONG      == col->datatype  ||
        OCI_CDT_LOB       == col->datatype  ||
        OCI_CDT_FILE      == col->datatype  ||
        OCI_CDT_TIMESTAMP == col->datatype  ||
        OCI_CDT_INTERVAL  == col->datatype)
    {
        type = col->subtype;
    }

    return type;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetAttrInfo
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ColumnGetAttrInfo
(
    OCI_Column    *col,
    unsigned int   count,
    unsigned int   index,
    size_t        *p_size,
    int           *p_type
)
{
    if (index >= count)
    {
        *p_size = 0;
        *p_type = 0;

        return FALSE;
    }

    switch (col->datatype)
    {
        case OCI_CDT_NUMERIC:
        {
            int type = col->subtype;

            if (type & OCI_NUM_SHORT)
            {
                *p_type = OCI_OFT_SHORT;
                *p_size = sizeof(short);
            }
            else if (type & OCI_NUM_INT)
            {
                *p_type = OCI_OFT_INT;
                *p_size = sizeof(int);
            }
            else if (type & OCI_NUM_BIGUINT)
            {
                *p_type = OCI_OFT_BIGINT;
                *p_size = sizeof(big_int);
            }
            else if (type & OCI_NUM_DOUBLE)
            {
                *p_type = OCI_OFT_DOUBLE;
                *p_size = sizeof(double);
            }
            else if (type & OCI_NUM_FLOAT)
            {
                *p_type = OCI_OFT_FLOAT;
                *p_size = sizeof(float);
            }
            else 
            {
                /* default mapping to big_int */

                *p_type = OCI_OFT_BIGINT;
                *p_size = sizeof(big_int);
            }
            break;
        }
        case OCI_CDT_OBJECT:
        {
            *p_size = OCI_ObjectGetUserStructSize(col->typinf);
            *p_type = OCI_OFT_STRUCT;
            break;
        }
        default:
        {
            *p_size = sizeof(void *);
            *p_type = OCI_OFT_POINTER;
            break;
        }
    }

    return TRUE;
}
