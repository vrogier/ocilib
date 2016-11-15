/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2016 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
* OCI_ColumnGetAttrInfo
* --------------------------------------------------------------------------------------------- */

boolean OCI_ColumnGetAttrInfo
(
    OCI_Column    *col,
    unsigned int   count,
    unsigned int   index,
    size_t        *p_size,
    size_t        *p_align
)
{
    if (index >= count)
    {
        *p_size = 0;

        return FALSE;
    }

    switch (col->datatype)
    {
        case OCI_CDT_BOOLEAN:
        {
            *p_size = sizeof(boolean);
            break;
        }
        case OCI_CDT_NUMERIC:
        {
            int type = col->subtype;

            if (type & OCI_NUM_SHORT)
            {
                *p_size = sizeof(short);
            }
            else if (type & OCI_NUM_INT)
            {
                *p_size = sizeof(int);
            }
            else if (type & OCI_NUM_BIGUINT)
            {
                *p_size = sizeof(big_int);
            }
            else if (type & OCI_NUM_DOUBLE)
            {
                *p_size = sizeof(double);
            }
            else if (type & OCI_NUM_FLOAT)
            {
                *p_size = sizeof(float);
            }
            else
            {
                /* default mapping to big_int */

                *p_size = sizeof(big_int);
            }

            *p_align = *p_size;
            break;
        }
        case OCI_CDT_OBJECT:
        {
            OCI_ObjectGetUserStructSize(col->typinf, p_size, p_align);
            break;
        }
        default:
        {
            *p_size  = sizeof(void *);
            *p_align = *p_size;
            break;
        }
    }

    return TRUE;
}

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
    void *param = NULL;

    OCI_CALL_DECLARE_CONTEXT(TRUE)

    if (stmt)
    {
        OCI_CALL_CONTEXT_SET_FROM_STMT(stmt);
    }
    else
    {
        OCI_CALL_CONTEXT_SET_FROM_CONN(con);
    }

    /* get descriptor */

    if (OCI_DESC_COLLECTION == ptype)
    {
        OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_COLLECTION_ELEMENT, handle, &param, NULL)
    }
    else
    {
        ub4 htype = (OCI_DESC_RESULTSET == ptype) ? OCI_HTYPE_STMT : OCI_DTYPE_PARAM;

        OCI_EXEC(OCIParamGet((dvoid *) handle, htype,  con->err, (void**) &param, (ub4) index))
    }

    /* sql code */
    OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_DATA_TYPE, param, &col->sqlcode, NULL)

    /* when the column is a record from a PL/SQL table, OCI returns an undocumented SQLT code */

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (SQLT_UNDOCUMENTED_REC == col->sqlcode)
    {
        col->sqlcode = SQLT_REC;
    }

#endif

    /* size */

    OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_DATA_SIZE, param, &col->size, NULL)

    /* scale */

    OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_SCALE, param, &col->scale, NULL)

    /* precision */

    if (OCI_DESC_RESULTSET == ptype)
    {
        sb2 prec = 0;

        OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_PRECISION, param, &prec, NULL)

        col->prec = (sb2) prec;
    }
    else
    {
        ub1 prec = 0;

        OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_PRECISION, param, &prec, NULL)

        col->prec = (sb2) prec;
    }

    /* charset form */

    OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_CHARSET_FORM, param, &col->csfrm, NULL)

    /* type of column length for string based column */

#if OCI_VERSION_COMPILE >= OCI_9_2

    if ((OCILib.version_runtime >= OCI_9_2) && (con->ver_num >= OCI_9_2))
    {
        OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_CHAR_USED, param, &col->charused, NULL)
    }

    /* char size */

    if (col->charused)
    {
        OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_CHAR_SIZE, param, &col->charsize, NULL)
    }

    if ((OCILib.version_runtime >= OCI_9_0) && (con->ver_num >= OCI_9_0))
    {
        /* fractional time precision for timestamps */

        if ( SQLT_TIMESTAMP     == col->sqlcode ||
             SQLT_TIMESTAMP_TZ  == col->sqlcode ||
             SQLT_TIMESTAMP_LTZ == col->sqlcode )
        {
            OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_FSPRECISION, param, &col->prec, NULL)
        }

        /* leading and fractional precision for interval */

        if (SQLT_INTERVAL_DS == col->sqlcode  || SQLT_INTERVAL_YM == col->sqlcode)
        {
            OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_LFPRECISION, param, &col->prec, NULL)
            OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_FSPRECISION, param, &col->prec2, NULL)
        }
    }

#endif

#if OCI_VERSION_COMPILE >= OCI_12_1

    if ((OCILib.version_runtime >= OCI_12_1) && (con->ver_num >= OCI_12_1))
    {
        if (ptype < OCI_DESC_TYPE)
        {
            ub8 value = 0;

            OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_COL_PROPERTIES, param, &value, NULL)

            if (value & OCI_ATTR_COL_PROPERTY_IS_IDENTITY)
            {
                col->props |=  OCI_CPF_IS_IDENTITY;
            }

            if (value & OCI_ATTR_COL_PROPERTY_IS_GEN_ALWAYS)
            {
                col->props |=  OCI_CPF_IS_GEN_ALWAYS;
            }

            if (value & OCI_ATTR_COL_PROPERTY_IS_GEN_BY_DEF_ON_NULL)
            {
                col->props |=  OCI_CPF_IS_GEN_BY_DEFAULT_ON_NULL;
            }
        }
    }

#endif

    /* check nullable only for table based column */

    if (ptype < OCI_DESC_TYPE)
    {
        OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_IS_NULL, param, &col->nullable, NULL)
    }
    else
    {
        col->nullable = TRUE;
    }

    /* name */

    if (OCI_STATUS)
    {

#if defined(OCI_CHARSET_WIDE)

        // Ugly workaround for Oracle Bug 9838993

        if ((OCI_DESC_RESULTSET == ptype) && (OCILib.env_vars[OCI_VARS_WORKAROUND_UTF16_COLUMN_NAME]))
        {
            OCIParamStruct *param_struct = (OCIParamStruct*) param;

            if (param_struct && param_struct->column_info && param_struct->column_info->name)
            {
                size_t char_count = OCI_StringLength(param_struct->column_info->name, sizeof(char));

                OCI_ALLOCATE_DATA(OCI_IPC_STRING, col->name, char_count + 1)

                if (OCI_STATUS)
                {
                    OCI_StringAnsiToNative(param_struct->column_info->name, col->name, (int) char_count);
                }
            }
            else
            {
                OCI_STATUS = FALSE;
            }
        }
        else

#endif

        {
            dbtext *dbstr    = NULL;
            int     dbsize   = 0;
            ub4     attrname = (OCI_DESC_COLLECTION == ptype) ? OCI_ATTR_TYPE_NAME : OCI_ATTR_NAME;

            OCI_GET_ATTRIB(OCI_DTYPE_PARAM, attrname, param, &dbstr, &dbsize)

            if (OCI_STATUS && dbstr)
            {
                col->name = OCI_StringDuplicateFromOracleString(dbstr, dbcharcount(dbsize));

                OCI_STATUS = (NULL != col->name);
            }
        }
    }

    /* user type descriptor */

    if (
        SQLT_NTY == col->sqlcode ||
        SQLT_REF == col->sqlcode
#if OCI_VERSION_COMPILE >= OCI_12_1
        || SQLT_REC == col->sqlcode
        || SQLT_TAB == col->sqlcode
#endif
    )
    {
        dbtext *dbstr_name    = NULL;
        dbtext *dbstr_schema  = NULL;
        dbtext *dbstr_package = NULL;
        int    dbsize_name    = 0;
        int    dbsize_schema  = 0;
        int    dbsize_package = 0;

        OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_TYPE_NAME, param, &dbstr_name, &dbsize_name)
        OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_SCHEMA_NAME, param, &dbstr_schema, &dbsize_schema)

    #if OCI_VERSION_COMPILE >= OCI_12_1


        if (SQLT_REC == col->sqlcode || SQLT_TAB == col->sqlcode)
        {
            OCI_GET_ATTRIB(OCI_DTYPE_PARAM, OCI_ATTR_PACKAGE_NAME, handle, &dbstr_package, &dbsize_package)
        }

    #endif


        if (OCI_STATUS)
        {
            otext schema_name[OCI_SIZE_OBJ_NAME + 1] = OTEXT("");
            otext package_name[OCI_SIZE_OBJ_NAME + 1] = OTEXT("");
            otext type_name[OCI_SIZE_OBJ_NAME + 1] = OTEXT("");
            otext full_name[(OCI_SIZE_OBJ_NAME * 3) + 3] = OTEXT("");

            /* Retrieve correct schema name */

            if (dbstr_schema && (dbsize_schema > 0))
            {
                OCI_StringOracleToNative(dbstr_schema, schema_name, dbcharcount(dbsize_schema));

                if (0 == ostrcasecmp(dbstr_schema, OTEXT("PUBLIC")))
                {
                    schema_name[0] = 0;
                }
            }

            /* Retrieve correct type name */

            if (dbstr_package && (dbsize_package > 0))
            {
                OCI_StringOracleToNative(dbstr_package, package_name, dbcharcount(dbsize_package));
            }

            /* Retrieve correct type name */

            if (dbstr_name && (dbsize_name > 0))
            {
                OCI_StringOracleToNative(dbstr_name, type_name, dbcharcount(dbsize_name));
            }

            /* Format full type name respecting case sensitivity if needed in order to not fail type info retrieval.*/

            OCI_StringGetFullTypeName(schema_name, package_name, type_name, NULL, full_name, (sizeof(full_name) / sizeof(otext)) - 1);

            col->typinf = OCI_TypeInfoGet(con, full_name, OCI_TIF_TYPE);

            OCI_STATUS = (NULL != col->typinf);
        }
    }

    if (param)
    {
        OCI_STATUS = (OCI_SUCCESS == OCIDescriptorFree(param, OCI_DTYPE_PARAM) && OCI_STATUS);
    }

    return OCI_STATUS;
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

    OCI_CHECK(NULL == col, FALSE)

    /* map Oracle SQL code to OCILIB types and setup of internal buffer size */

    col->libcode = col->sqlcode;

    switch (col->libcode)
    {
        case SQLT_INT:
        case SQLT_UNDOCUMENTED_BIN_INTEGER:
#if OCI_VERSION_COMPILE >= OCI_12_1
        case OCI_TYPECODE_PLS_INTEGER:
#endif
        {
            col->datatype = OCI_CDT_NUMERIC;
            col->libcode  = SQLT_INT;
            /* set buffer size only if it's not a "returning into" placeholder */

            if (0 == col->bufsize)
            {
                col->subtype = OCI_NUM_INT;
                col->bufsize = sizeof(int);
            }

            if (0 == col->size)
            {
                col->size = sizeof(int);
            }
            break;
        }
        case SQLT_UIN:
        {
            col->datatype = OCI_CDT_NUMERIC;

            /* set buffer size only if it's not a "returning into" placeholder */

            if (0 == col->bufsize)
            {
                col->subtype = OCI_NUM_UINT;
                col->bufsize = sizeof(unsigned int);
            }

            if (0 == col->size)
            {
                col->size = sizeof(unsigned int);
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

    #if OCI_VERSION_COMPILE >= OCI_12_1

        case SQLT_BOL:

        {
            col->datatype = OCI_CDT_BOOLEAN;
            col->subtype  = 0;
            col->libcode  = SQLT_BOL;
            col->bufsize  = sizeof(boolean);
            break;
        }

    #endif

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

            if (0 == col->bufsize)
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

    #if OCI_VERSION_COMPILE >= OCI_12_1

        case SQLT_REC:
        {
            col->libcode  = SQLT_REC;
            col->bufsize  = (ub4) sizeof(void *);
            col->datatype = OCI_CDT_OBJECT;
            break;
        }

    #endif

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
    OCI_GET_PROP(const otext*, NULL, OCI_IPC_COLUMN, col, name, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ColumnGetType
(
    OCI_Column *col
)
{
    OCI_GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_COLUMN, col, datatype, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetCharsetForm
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ColumnGetCharsetForm
(
    OCI_Column *col
)
{
    OCI_CALL_ENTER(unsigned int, OCI_CSF_NONE)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    if (SQLCS_NCHAR == col->csfrm)
    {
        OCI_RETVAL = OCI_CSF_NATIONAL;
    }
    else if (SQLCS_IMPLICIT == col->csfrm)
    {
        OCI_RETVAL = OCI_CSF_DEFAULT;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ColumnGetSize
(
    OCI_Column *col
)
{
    OCI_CALL_ENTER(unsigned int, 0)

    OCI_CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    /* Oracle 9i introduced CHAR attribute on string columns to indicate the
       size of the column is not in bytes (default) but in chars
       OCI_ColumnDescribe() already managed the Oracle compatibly
       version, so if the column character size member is zero it means :
       - the column is not a string column
       - the size is not in char
       - client does not support the OCI_ATTR_CHAR_SIZE attribute */

    OCI_RETVAL = (col->charused && col->charsize > 0) ? col->charsize : col->size;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetScale
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ColumnGetScale
(
    OCI_Column *col
)
{
    OCI_GET_PROP(int, 0, OCI_IPC_COLUMN, col, scale, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetPrecision
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ColumnGetPrecision
(
    OCI_Column *col
)
{
    OCI_CALL_ENTER(int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    OCI_RETVAL = (OCI_CDT_NUMERIC == col->datatype) ? (int) col->prec : 0;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetFractionalPrecision
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ColumnGetFractionalPrecision
(
    OCI_Column *col
)
{
    OCI_CALL_ENTER(int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    if (OCI_CDT_TIMESTAMP == col->datatype)
    {
        OCI_RETVAL = (int) col->prec;
    }
    else if (OCI_CDT_INTERVAL == col->datatype)
    {
        OCI_RETVAL = (int)col->prec2;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetLeadingPrecision
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ColumnGetLeadingPrecision
(
    OCI_Column *col
)
{
    OCI_CALL_ENTER(int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    OCI_RETVAL = (OCI_CDT_INTERVAL == col->datatype) ? (int) col->prec : 0;
    OCI_STATUS = TRUE;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetNullable
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ColumnGetNullable
(
    OCI_Column *col
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    OCI_RETVAL = (1 == col->nullable);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetCharUsed
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ColumnGetCharUsed
(
    OCI_Column *col
)
{
    OCI_GET_PROP(boolean, FALSE, OCI_IPC_COLUMN, col, charused, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetPropertyFlags
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ColumnGetPropertyFlags
(
    OCI_Column *col
)
{
    OCI_GET_PROP(unsigned int, 0, OCI_IPC_COLUMN, col, props, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetSQLType
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_ColumnGetSQLType
(
    OCI_Column *col
)
{
    OCI_CALL_ENTER(const otext*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    /* VARCHAR type will not be returned because Oracle does not make any
       difference with VARCHAR2. If a column is created with VARCHAR, it is
       internally created as VARCHAR2
    */

    switch(col->sqlcode)
    {
        case SQLT_AFC:
        {
            OCI_RETVAL = (SQLCS_NCHAR == col->csfrm) ? OTEXT("NCHAR") : OTEXT("CHAR");;
            break;
        }
        case SQLT_AVC:
        case SQLT_STR:
        case SQLT_CHR:
        {
             OCI_RETVAL = (SQLCS_NCHAR == col->csfrm) ? OTEXT("NVARCHAR2") : OTEXT("VARCHAR2");
             break;
        }
        case SQLT_NUM:
        {
            OCI_RETVAL = (SCALE_FLOAT == col->scale && col->prec > 0) ? OTEXT("FLOAT") : OTEXT("NUMBER");
            break;
        }
        case SQLT_INT:
        {
            OCI_RETVAL = OTEXT("INTEGER");
            break;
        }
        case SQLT_FLT:
        {
            OCI_RETVAL = OTEXT("FLOAT");
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_10_1

        case SQLT_BFLOAT:
        case SQLT_IBFLOAT:
        {
            OCI_RETVAL = OTEXT("BINARY FLOAT");
            break;
        }
        case SQLT_BDOUBLE:
        case SQLT_IBDOUBLE:
        {
            OCI_RETVAL = OTEXT("BINARY DOUBLE");
            break;
        }

    #endif

        case SQLT_LNG:
        {
            OCI_RETVAL = OTEXT("LONG");
            break;
        }
        case SQLT_DAT:
        case SQLT_ODT:
        case SQLT_DATE:
        {
            OCI_RETVAL = OTEXT("DATE");
            break;
        }
        case SQLT_RDD:
        case SQLT_RID:
        {
            OCI_RETVAL = OTEXT("ROWID");
            break;
        }
        case SQLT_BIN:
        {
            OCI_RETVAL = OTEXT("RAW");
            break;
        }
        case SQLT_LBI:
        {
            OCI_RETVAL = OTEXT("LONG RAW");
            break;
        }
        case SQLT_RSET:
        {
            OCI_RETVAL = OTEXT("RESULTSET");
            break;
        }
        case SQLT_CUR:
        {
            OCI_RETVAL = OTEXT("CURSOR");
            break;
        }
        case SQLT_CLOB:
        {
            OCI_RETVAL = (col->subtype == OCI_NCLOB) ? OTEXT("NCLOB") : OTEXT("CLOB");
            break;
        }
        case SQLT_BLOB:
        {
            OCI_RETVAL = OTEXT("BLOB");
            break;
        }
        case SQLT_BFILE:
        {
            OCI_RETVAL = OTEXT("BINARY FILE LOB");
            break;
        }
        case SQLT_CFILE:
        {
            OCI_RETVAL = OTEXT("CFILE");
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_TIMESTAMP:
        {
            OCI_RETVAL = OTEXT("TIMESTAMP");
            break;
        }
        case SQLT_TIMESTAMP_TZ:
        {
            OCI_RETVAL = OTEXT("TIMESTAMP WITH TIME ZONE");
            break;
        }
        case SQLT_TIMESTAMP_LTZ:
        {
            OCI_RETVAL = OTEXT("TIMESTAMP WITH LOCAL TIME ZONE");
            break;
        }
        case SQLT_INTERVAL_YM:
        {
            OCI_RETVAL = OTEXT("INTERVAL YEAR TO MONTH");
            break;
        }
        case SQLT_INTERVAL_DS:
        {
            OCI_RETVAL = OTEXT("INTERVAL DAY TO SECOND");
            break;
        }

    #endif

        case SQLT_REF:
        {
            OCI_RETVAL = OTEXT("REF");
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_PNTY:

    #endif

        case SQLT_NTY:
        {
            OCI_RETVAL = col->typinf ? col->typinf->name : OTEXT("NAMED TYPE");
            break;
        }



#if OCI_VERSION_COMPILE >= OCI_12_1

        case SQLT_BOL:
        {
            OCI_RETVAL = OTEXT("PL/SQL BOOLEAN");
            break;
        }

        case SQLT_REC:
        case SQLT_UNDOCUMENTED_REC:
        {
            OCI_RETVAL = col->typinf ? col->typinf->name : OTEXT("PL/SQL RECORD");
            break;
        }

        case SQLT_TAB:
        {
            OCI_RETVAL = col->typinf ? col->typinf->name : OTEXT("PL/SQL TABLE INDEX BY");
            break;
        }

        case SQLT_UNDOCUMENTED_BIN_INTEGER:
        {
            OCI_RETVAL = OTEXT("PL/SQL BINARY INTEGER");
            break;
        }

        case OCI_TYPECODE_PLS_INTEGER:
        {
            OCI_RETVAL = OTEXT("PL/SQL INTEGER");
            break;
        }

#endif

        default:
        {
            /* unknown data type ? Should not happen because all
                 data types are supported */

            OCI_RETVAL = OTEXT("?");
        }
    }

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(unsigned int, 0)

    OCI_CALL_CHECK_PTR(OCI_IPC_COLUMN, col)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, buffer)

    buffer[0] = 0;

    /* ISO C functions are supposed to be "standard", but we still see specific
       implementations that make some usage not portable and worse not compatible.
       MS Windows is implementing string conversion tokens (%s/%ls) of the standard
       library formatting functions differently from UNIX systems !
    */

    /* This function returns the same strings as Sql*Plus DESC command */

    switch(col->sqlcode)
    {
        case SQLT_AFC:
        {

        #if defined(OCI_CHARSET_WIDE) && !defined(_WINDOWS)
            OCI_RETVAL = osprintf(buffer, len, OTEXT("%lsCHAR(%i%ls)"),
        #else
            OCI_RETVAL = osprintf(buffer, len, OTEXT("%sCHAR(%i%s)"),
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
            const otext *fmt = OTEXT("%lsVARCHAR(%i%ls)");
         #else
            const otext *fmt = OTEXT("%sVARCHAR(%i%s)");
         #endif

            OCI_RETVAL = osprintf(buffer, len, fmt, SQLCS_NCHAR == col->csfrm ? OTEXT("N") : OTEXT(""),
                                   (int) (col->charused ? col->charsize : col->size),
                                   col->charused &&  SQLCS_NCHAR != col->csfrm ? OTEXT(" CHAR") : OTEXT(""));
            break;
        }
        case SQLT_NUM:
        {
            if (SCALE_FLOAT == col->scale && col->prec > 0)
            {
                OCI_RETVAL = osprintf(buffer, len,  OTEXT("FLOAT(%i)"), col->prec);
            }
            else if (col->scale > 0 && col->prec > 0)
            {
                OCI_RETVAL = osprintf(buffer, len,  OTEXT("NUMBER(%i,%i)"), (int) col->prec, (int) col->scale);
            }
            else if (col->prec > 0)
            {
                OCI_RETVAL = osprintf(buffer, len,  OTEXT("NUMBER(%i)"), (int) col->prec);
            }
            else
            {
                OCI_RETVAL = osprintf(buffer, len,  OTEXT("NUMBER"));
            }

            break;
        }
        case SQLT_INT:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("NUMBER"));
            break;
        }
        case SQLT_FLT:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("FLOAT(%i)"), (int) col->prec);
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_10_1

        case SQLT_BFLOAT:
        case SQLT_IBFLOAT:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("BINARY FLOAT"));
            break;
        }
        case SQLT_BDOUBLE:
        case SQLT_IBDOUBLE:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("BINARY DOUBLE"));
            break;
        }

    #endif

        case SQLT_LNG:
        {
            OCI_RETVAL = osprintf(buffer, len, OTEXT("LONG"));
            break;
        }
        case SQLT_DAT:
        case SQLT_ODT:
        case SQLT_DATE:
        {
            OCI_RETVAL = osprintf(buffer, len, OTEXT("DATE"));
            break;
        }
        case SQLT_RDD:
        case SQLT_RID:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("ROWID"));
            break;
        }
        case SQLT_BIN:
        {
            OCI_RETVAL = osprintf(buffer, len, OTEXT("RAW(%i)"), (int) col->size);
            break;
        }
        case SQLT_LBI:
        {
            OCI_RETVAL = osprintf(buffer, len, OTEXT("LONG RAW(%i)"), (int) col->size);
            break;
        }
        case SQLT_RSET:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("RESULTSET"));
            break;
        }
        case SQLT_CUR:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("CURSOR"));
            break;
        }
        case SQLT_CLOB:
        {
            OCI_RETVAL = osprintf(buffer, len, (col->subtype == OCI_NCLOB) ? OTEXT("NCLOB") : OTEXT("CLOB"));
            break;
        }
        case SQLT_BLOB:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("BLOB"));
            break;
        }
        case SQLT_BFILE:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("BINARY FILE LOB"));
            break;
        }
        case SQLT_CFILE:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("CFILE"));
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_TIMESTAMP:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("TIMESTAMP(%i)"), (int) col->prec);
            break;
        }
        case SQLT_TIMESTAMP_TZ:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("TIMESTAMP(%i) WITH TIME ZONE"), (int) col->prec);
            break;
        }
        case SQLT_TIMESTAMP_LTZ:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("TIMESTAMP(%i) WITH LOCAL TIME ZONE"), (int) col->prec);
            break;
        }
        case SQLT_INTERVAL_YM:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("INTERVAL(%i) YEAR TO MONTH(%i)"),
                                  (int) col->prec, (int) col->prec2);
            break;
        }
        case SQLT_INTERVAL_DS:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("INTERVAL(%i) DAY TO SECOND(%i)"),
                                  (int) col->prec, (int) col->prec2);
            break;
        }

    #endif

        case SQLT_REF:
        {
            OCI_RETVAL = osprintf(buffer, len,  OTEXT("REF"));
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_PNTY:

    #endif

        case SQLT_NTY:
        {
            OCI_RETVAL = osprintf(buffer, len, col->typinf ? col->typinf->name : OTEXT("NAMED TYPE"));
            break;
        }


#if OCI_VERSION_COMPILE >= OCI_12_1

        case SQLT_BOL:
        {
            OCI_RETVAL = osprintf(buffer, len, OTEXT("PL/SQL BOOLEAN"));
            break;
        }

        case SQLT_REC:
        case SQLT_UNDOCUMENTED_REC:
        {
            OCI_RETVAL = osprintf(buffer, len, col->typinf ? col->typinf->name : OTEXT("PL/SQL RECORD"));
            break;
        }

        case SQLT_TAB:
        {
            OCI_RETVAL = osprintf(buffer, len, col->typinf ? col->typinf->name : OTEXT("PL/SQL TABLE INDEX BY"));
            break;
        }

        case SQLT_UNDOCUMENTED_BIN_INTEGER:
        {
            OCI_RETVAL = osprintf(buffer, len, OTEXT("PL/SQL BINARY INTEGER"));
            break;
        }

        case OCI_TYPECODE_PLS_INTEGER:
        {
            OCI_RETVAL = osprintf(buffer, len, OTEXT("PL/SQL INTEGER"));
            break;
        }

#endif

        default:
        {
            OCI_RETVAL = osprintf(buffer, len, OTEXT("?"));
            break;
        }
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OCI_API OCI_ColumnGetTypeInfo
(
    OCI_Column *col
)
{
    OCI_GET_PROP(OCI_TypeInfo*, NULL, OCI_IPC_COLUMN, col, typinf, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ColumnGetSubType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ColumnGetSubType
(
    OCI_Column *col
)
{
    OCI_CALL_ENTER(unsigned int, OCI_UNKNOWN)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    if (OCI_CDT_LONG      == col->datatype  ||
        OCI_CDT_LOB       == col->datatype  ||
        OCI_CDT_FILE      == col->datatype  ||
        OCI_CDT_TIMESTAMP == col->datatype  ||
        OCI_CDT_INTERVAL  == col->datatype)
    {
        OCI_RETVAL = col->subtype;
    }

    OCI_CALL_EXIT()
}

