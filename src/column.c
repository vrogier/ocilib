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

#include "column.h"

#include "connection.h"
#include "macros.h"
#include "object.h"
#include "strings.h"
#include "typeinfo.h"

/* --------------------------------------------------------------------------------------------- *
* ColumnGetAttrInfo
* --------------------------------------------------------------------------------------------- */

boolean ColumnGetAttributeInfo
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
            const unsigned int type = col->subtype;

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
            ObjectGetUserStructSize(col->typinf, p_size, p_align);
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
 * ColumnRetrieveInfo
 * --------------------------------------------------------------------------------------------- */

boolean ColumnRetrieveInfo
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

    DECLARE_CTX(TRUE)

    if (stmt)
    {
        CALL_CONTEXT_FROM_STMT(stmt);
    }
    else
    {
        CALL_CONTEXT_FROM_CON(con);
    }

    /* get descriptor */

    if (OCI_DESC_COLLECTION == ptype)
    {
        ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_COLLECTION_ELEMENT, handle, &param, NULL)
    }
    else
    {
        const ub4 htype = (OCI_DESC_RESULTSET == ptype) ? OCI_HTYPE_STMT : OCI_DTYPE_PARAM;

        EXEC(OCIParamGet((dvoid *) handle, htype,  con->err, (void**) &param, (ub4) index))
    }

    /* sql code */
    ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_DATA_TYPE, param, &col->sqlcode, NULL)

    /* when the column is a record from a PL/SQL table, OCI returns an undocumented SQLT code */

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (SQLT_UNDOCUMENTED_REC == col->sqlcode)
    {
        col->sqlcode = SQLT_REC;
    }

#endif

    /* size */

    ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_DATA_SIZE, param, &col->size, NULL)

    /* scale */

    ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_SCALE, param, &col->scale, NULL)

    /* precision */

    if (OCI_DESC_RESULTSET == ptype)
    {
        sb2 prec = 0;

        ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_PRECISION, param, &prec, NULL)

        col->prec = (sb2) prec;
    }
    else
    {
        ub1 prec = 0;

        ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_PRECISION, param, &prec, NULL)

        col->prec = (sb2) prec;
    }

    /* charset form */

    ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_CHARSET_FORM, param, &col->csfrm, NULL)

    /* type of column length for string based column */

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (ConnectionIsVersionSupported(con, OCI_9_2))
    {
        ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_CHAR_USED, param, &col->charused, NULL)
    }

    /* char size */

    if (col->charused)
    {
        ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_CHAR_SIZE, param, &col->charsize, NULL)
    }

#endif

    if (ConnectionIsVersionSupported(con, OCI_9_0))
    {
        /* fractional time precision for timestamps */

        if ( SQLT_TIMESTAMP     == col->sqlcode ||
             SQLT_TIMESTAMP_TZ  == col->sqlcode ||
             SQLT_TIMESTAMP_LTZ == col->sqlcode )
        {
            ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_FSPRECISION, param, &col->prec, NULL)
        }

        /* leading and fractional precision for interval */

        if (SQLT_INTERVAL_DS == col->sqlcode  || SQLT_INTERVAL_YM == col->sqlcode)
        {
            ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_LFPRECISION, param, &col->prec, NULL)
            ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_FSPRECISION, param, &col->prec2, NULL)
        }
    }

#if OCI_VERSION_COMPILE >= OCI_12_2

    if (ConnectionIsVersionSupported(con, OCI_12_2))
    {
        if (OCI_DESC_RESULTSET == ptype)
        {
            ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_COLLATION_ID, param, &col->collation_id, NULL)
        }
    }

#endif

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (ConnectionIsVersionSupported(con, OCI_12_1))
    {
        if (ptype < OCI_DESC_TYPE)
        {
            ub8 value = 0;

            ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_COL_PROPERTIES, param, &value, NULL)

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

        #if OCI_VERSION_COMPILE >= OCI_18_1

            if (value & OCI_ATTR_COL_PROPERTY_IS_LPART)
            {
                col->props |= OCI_CPF_IS_LPART;
            }

            if (value & OCI_ATTR_COL_PROPERTY_IS_CONID)
            {
                col->props |= OCI_CPF_IS_CONID;
            }

        #endif 
        }
    }

#endif

    /* check nullable only for table based column */

    if (ptype < OCI_DESC_TYPE)
    {
        ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_IS_NULL, param, &col->nullable, NULL)
    }
    else
    {
        col->nullable = TRUE;
    }

    /* name */

    if (STATUS)
    {

#if defined(OCI_CHARSET_WIDE)

        // Ugly workaround for Oracle Bug 9838993

        if ((OCI_DESC_RESULTSET == ptype) && (Env.env_vars[OCI_VARS_WORKAROUND_UTF16_COLUMN_NAME]))
        {
            OCIParamStruct *param_struct = (OCIParamStruct*) param;

            if (param_struct && param_struct->column_info && param_struct->column_info->name && (param_struct->column_info->attributes[1] != 0))
            {
                size_t char_count = param_struct->column_info->attributes[1];

                ALLOC_DATA(OCI_IPC_STRING, col->name, char_count + 1)

                if (STATUS)
                {
                    StringAnsiToNative(param_struct->column_info->name, col->name, (int) char_count);
                }
            }
            else
            {
                STATUS = FALSE;
            }
        }
        else

#endif

        {
            int       dbsize   = 0;
            dbtext   *dbstr    = NULL;
            const ub4 attrname = (OCI_DESC_COLLECTION == ptype) ? OCI_ATTR_TYPE_NAME : OCI_ATTR_NAME;

            ATTRIB_GET(OCI_DTYPE_PARAM, attrname, param, &dbstr, &dbsize)

            if (STATUS && dbstr)
            {
                col->name = StringDuplicateFromDBString(dbstr, dbcharcount(dbsize));

                STATUS = (NULL != col->name);
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

        ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_TYPE_NAME, param, &dbstr_name, &dbsize_name)
        ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_SCHEMA_NAME, param, &dbstr_schema, &dbsize_schema)

    #if OCI_VERSION_COMPILE >= OCI_12_1


        if (SQLT_REC == col->sqlcode || SQLT_TAB == col->sqlcode)
        {
            ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_PACKAGE_NAME, handle, &dbstr_package, &dbsize_package)
        }

    #endif


        if (STATUS)
        {
            otext schema_name[OCI_SIZE_OBJ_NAME + 1] = OTEXT("");
            otext package_name[OCI_SIZE_OBJ_NAME + 1] = OTEXT("");
            otext type_name[OCI_SIZE_OBJ_NAME + 1] = OTEXT("");
            otext full_name[(OCI_SIZE_OBJ_NAME * 3) + 3] = OTEXT("");

            /* Retrieve correct schema name */

            if (dbstr_schema && (dbsize_schema > 0))
            {
                StringOracleToNative(dbstr_schema, schema_name, dbcharcount(dbsize_schema));

                if (0 == ostrcasecmp(schema_name, OTEXT("PUBLIC")))
                {
                    schema_name[0] = 0;
                }
            }

            /* Retrieve correct type name */

            if (dbstr_package && (dbsize_package > 0))
            {
                StringOracleToNative(dbstr_package, package_name, dbcharcount(dbsize_package));
            }

            /* Retrieve correct type name */

            if (dbstr_name && (dbsize_name > 0))
            {
                StringOracleToNative(dbstr_name, type_name, dbcharcount(dbsize_name));
            }

            /* Format full type name respecting case sensitivity if needed in order to not fail type info retrieval.*/

            StringGetFullTypeName(schema_name, package_name, type_name, NULL, full_name, (sizeof(full_name) / sizeof(otext)) - 1);

            col->typinf = TypeInfoGet(con, full_name, OCI_TIF_TYPE);

            STATUS = (NULL != col->typinf);
        }
    }

    if (param)
    {
        STATUS = (OCI_SUCCESS == OCIDescriptorFree(param, OCI_DTYPE_PARAM) && STATUS);
    }

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnMap
 * --------------------------------------------------------------------------------------------- */

boolean ColumnMapInfo
(
    OCI_Column    *col,
    OCI_Statement *stmt
)
{
    const ub2 char_size = (ub2) ( (Env.nls_utf8 ) ? OCI_UTF8_BYTES_PER_CHAR : sizeof(otext) );

    CHECK(NULL == col, FALSE)

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
                col->size = (ub2) col->bufsize;
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
                col->size = (ub2) col->bufsize;
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

            if (0 == col->size)
            {
                col->size = (ub2) col->bufsize;
            }
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_12_1

        case SQLT_BOL:

        {
            col->datatype = OCI_CDT_BOOLEAN;
            col->subtype  = 0;
            col->libcode  = SQLT_BOL;
            col->bufsize  = sizeof(boolean);

            if (0 == col->size)
            {
                col->size = (ub2) col->bufsize;
            }
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

            if (0 == col->size)
            {
                col->size = (ub2) col->bufsize;
            }
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

            if (0 == col->size)
            {
                col->size = (ub2) col->bufsize;
            }
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

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetName
 * --------------------------------------------------------------------------------------------- */

const otext * ColumnGetName
(
    OCI_Column *col
)
{
    GET_PROP(const otext*, NULL, OCI_IPC_COLUMN, col, name, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int ColumnGetType
(
    OCI_Column *col
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_COLUMN, col, datatype, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetCharsetForm
 * --------------------------------------------------------------------------------------------- */

unsigned int ColumnGetCharsetForm
(
    OCI_Column *col
)
{
    CALL_ENTER(unsigned int, OCI_CSF_NONE)
    CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    if (SQLCS_NCHAR == col->csfrm)
    {
        RETVAL = OCI_CSF_NATIONAL;
    }
    else if (SQLCS_IMPLICIT == col->csfrm)
    {
        RETVAL = OCI_CSF_DEFAULT;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int ColumnGetSize
(
    OCI_Column *col
)
{
    CALL_ENTER(unsigned int, 0)

    CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    /* Oracle 9i introduced CHAR attribute on string columns to indicate the
       size of the column is not in bytes (default) but in chars
       OCI_ColumnDescribe() already managed the Oracle compatibility
       version, so if the column character size member is zero it means :
       - the column is not a string column
       - the size is not in char
       - client does not support the OCI_ATTR_CHAR_SIZE attribute */

    RETVAL = (col->charused && col->charsize > 0) ? col->charsize : col->size;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetScale
 * --------------------------------------------------------------------------------------------- */

int ColumnGetScale
(
    OCI_Column *col
)
{
    GET_PROP(int, 0, OCI_IPC_COLUMN, col, scale, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetPrecision
 * --------------------------------------------------------------------------------------------- */

int ColumnGetPrecision
(
    OCI_Column *col
)
{
    CALL_ENTER(int, 0)
    CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    RETVAL = (OCI_CDT_NUMERIC == col->datatype) ? (int) col->prec : 0;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetFractionalPrecision
 * --------------------------------------------------------------------------------------------- */

int ColumnGetFractionalPrecision
(
    OCI_Column *col
)
{
    CALL_ENTER(int, 0)
    CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    if (OCI_CDT_TIMESTAMP == col->datatype)
    {
        RETVAL = (int) col->prec;
    }
    else if (OCI_CDT_INTERVAL == col->datatype)
    {
        RETVAL = (int)col->prec2;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetLeadingPrecision
 * --------------------------------------------------------------------------------------------- */

int ColumnGetLeadingPrecision
(
    OCI_Column *col
)
{
    CALL_ENTER(int, 0)
    CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    RETVAL = (OCI_CDT_INTERVAL == col->datatype) ? (int) col->prec : 0;
    STATUS = TRUE;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetNullable
 * --------------------------------------------------------------------------------------------- */

boolean ColumnGetNullable
(
    OCI_Column *col
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    RETVAL = (1 == col->nullable);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetCharUsed
 * --------------------------------------------------------------------------------------------- */

boolean ColumnGetCharUsed
(
    OCI_Column *col
)
{
    GET_PROP(boolean, FALSE, OCI_IPC_COLUMN, col, charused, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetPropertyFlags
 * --------------------------------------------------------------------------------------------- */

unsigned int ColumnGetPropertyFlags
(
    OCI_Column *col
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_COLUMN, col, props, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ColumnGetCollationID
* --------------------------------------------------------------------------------------------- */

unsigned int ColumnGetCollationID
(
    OCI_Column *col
)
{
    GET_PROP(unsigned int, OCI_CCI_NONE, OCI_IPC_COLUMN, col, collation_id, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetSQLType
 * --------------------------------------------------------------------------------------------- */

const otext * ColumnGetSqlType
(
    OCI_Column *col
)
{
    CALL_ENTER(const otext*, NULL)
    CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    /* VARCHAR type will not be returned because Oracle does not make any
       difference with VARCHAR2. If a column is created with VARCHAR, it is
       internally created as VARCHAR2
    */

    switch(col->sqlcode)
    {
        case SQLT_AFC:
        {
            RETVAL = (SQLCS_NCHAR == col->csfrm) ? OTEXT("NCHAR") : OTEXT("CHAR");;
            break;
        }
        case SQLT_AVC:
        case SQLT_STR:
        case SQLT_CHR:
        {
             RETVAL = (SQLCS_NCHAR == col->csfrm) ? OTEXT("NVARCHAR2") : OTEXT("VARCHAR2");
             break;
        }
        case SQLT_NUM:
        {
            RETVAL = (SCALE_FLOAT == col->scale && col->prec > 0) ? OTEXT("FLOAT") : OTEXT("NUMBER");
            break;
        }
        case SQLT_INT:
        {
            RETVAL = OTEXT("INTEGER");
            break;
        }
        case SQLT_FLT:
        {
            RETVAL = OTEXT("FLOAT");
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_10_1

        case SQLT_BFLOAT:
        case SQLT_IBFLOAT:
        {
            RETVAL = OTEXT("BINARY FLOAT");
            break;
        }
        case SQLT_BDOUBLE:
        case SQLT_IBDOUBLE:
        {
            RETVAL = OTEXT("BINARY DOUBLE");
            break;
        }

    #endif

        case SQLT_LNG:
        {
            RETVAL = OTEXT("LONG");
            break;
        }
        case SQLT_DAT:
        case SQLT_ODT:
        case SQLT_DATE:
        {
            RETVAL = OTEXT("DATE");
            break;
        }
        case SQLT_RDD:
        case SQLT_RID:
        {
            RETVAL = OTEXT("ROWID");
            break;
        }
        case SQLT_BIN:
        {
            RETVAL = OTEXT("RAW");
            break;
        }
        case SQLT_LBI:
        {
            RETVAL = OTEXT("LONG RAW");
            break;
        }
        case SQLT_RSET:
        {
            RETVAL = OTEXT("RESULTSET");
            break;
        }
        case SQLT_CUR:
        {
            RETVAL = OTEXT("CURSOR");
            break;
        }
        case SQLT_CLOB:
        {
            RETVAL = (col->subtype == OCI_NCLOB) ? OTEXT("NCLOB") : OTEXT("CLOB");
            break;
        }
        case SQLT_BLOB:
        {
            RETVAL = OTEXT("BLOB");
            break;
        }
        case SQLT_BFILE:
        {
            RETVAL = OTEXT("BINARY FILE LOB");
            break;
        }
        case SQLT_CFILE:
        {
            RETVAL = OTEXT("CFILE");
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_TIMESTAMP:
        {
            RETVAL = OTEXT("TIMESTAMP");
            break;
        }
        case SQLT_TIMESTAMP_TZ:
        {
            RETVAL = OTEXT("TIMESTAMP WITH TIME ZONE");
            break;
        }
        case SQLT_TIMESTAMP_LTZ:
        {
            RETVAL = OTEXT("TIMESTAMP WITH LOCAL TIME ZONE");
            break;
        }
        case SQLT_INTERVAL_YM:
        {
            RETVAL = OTEXT("INTERVAL YEAR TO MONTH");
            break;
        }
        case SQLT_INTERVAL_DS:
        {
            RETVAL = OTEXT("INTERVAL DAY TO SECOND");
            break;
        }

    #endif

        case SQLT_REF:
        {
            RETVAL = OTEXT("REF");
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_PNTY:

    #endif

        case SQLT_NTY:
        {
            RETVAL = col->typinf ? col->typinf->name : OTEXT("NAMED TYPE");
            break;
        }



#if OCI_VERSION_COMPILE >= OCI_12_1

        case SQLT_BOL:
        {
            RETVAL = OTEXT("PL/SQL BOOLEAN");
            break;
        }

        case SQLT_REC:
        case SQLT_UNDOCUMENTED_REC:
        {
            RETVAL = col->typinf ? col->typinf->name : OTEXT("PL/SQL RECORD");
            break;
        }

        case SQLT_TAB:
        {
            RETVAL = col->typinf ? col->typinf->name : OTEXT("PL/SQL TABLE INDEX BY");
            break;
        }

        case SQLT_UNDOCUMENTED_BIN_INTEGER:
        {
            RETVAL = OTEXT("PL/SQL BINARY INTEGER");
            break;
        }

        case OCI_TYPECODE_PLS_INTEGER:
        {
            RETVAL = OTEXT("PL/SQL INTEGER");
            break;
        }

#endif

        default:
        {
            /* unknown data type ? Should not happen because all
                 data types are supported */

            RETVAL = OTEXT("?");
        }
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetFullSQLType
 * --------------------------------------------------------------------------------------------- */

unsigned int ColumnGetFullSqlType
(
    OCI_Column  *col,
    otext       *buffer,
    unsigned int len
)
{
    CALL_ENTER(unsigned int, 0)

    CALL_CHECK_PTR(OCI_IPC_COLUMN, col)
    CALL_CHECK_PTR(OCI_IPC_STRING, buffer)

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
            RETVAL = osprintf(buffer, (int)len, OTEXT("%lsCHAR(%i%ls)"),
        #else
            RETVAL = osprintf(buffer, (int)len, OTEXT("%sCHAR(%i%s)"),
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

            RETVAL = osprintf(buffer, (int)len, fmt, SQLCS_NCHAR == col->csfrm ? OTEXT("N") : OTEXT(""),
                                   (int) (col->charused ? col->charsize : col->size),
                                   col->charused &&  SQLCS_NCHAR != col->csfrm ? OTEXT(" CHAR") : OTEXT(""));
            break;
        }
        case SQLT_NUM:
        {
            if (SCALE_FLOAT == col->scale && col->prec > 0)
            {
                RETVAL = osprintf(buffer, (int)len,  OTEXT("FLOAT(%i)"), col->prec);
            }
            else if (col->scale > 0 && col->prec > 0)
            {
                RETVAL = osprintf(buffer, (int)len,  OTEXT("NUMBER(%i,%i)"), (int) col->prec, (int) col->scale);
            }
            else if (col->prec > 0)
            {
                RETVAL = osprintf(buffer, (int)len,  OTEXT("NUMBER(%i)"), (int) col->prec);
            }
            else
            {
                RETVAL = osprintf(buffer, (int)len,  OTEXT("NUMBER"));
            }

            break;
        }
        case SQLT_INT:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("NUMBER"));
            break;
        }
        case SQLT_FLT:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("FLOAT(%i)"), (int) col->prec);
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_10_1

        case SQLT_BFLOAT:
        case SQLT_IBFLOAT:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("BINARY FLOAT"));
            break;
        }
        case SQLT_BDOUBLE:
        case SQLT_IBDOUBLE:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("BINARY DOUBLE"));
            break;
        }

    #endif

        case SQLT_LNG:
        {
            RETVAL = osprintf(buffer, (int)len, OTEXT("LONG"));
            break;
        }
        case SQLT_DAT:
        case SQLT_ODT:
        case SQLT_DATE:
        {
            RETVAL = osprintf(buffer, (int)len, OTEXT("DATE"));
            break;
        }
        case SQLT_RDD:
        case SQLT_RID:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("ROWID"));
            break;
        }
        case SQLT_BIN:
        {
            RETVAL = osprintf(buffer, (int)len, OTEXT("RAW(%i)"), (int) col->size);
            break;
        }
        case SQLT_LBI:
        {
            RETVAL = osprintf(buffer, (int)len, OTEXT("LONG RAW(%i)"), (int) col->size);
            break;
        }
        case SQLT_RSET:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("RESULTSET"));
            break;
        }
        case SQLT_CUR:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("CURSOR"));
            break;
        }
        case SQLT_CLOB:
        {
            RETVAL = osprintf(buffer, (int)len, (col->subtype == OCI_NCLOB) ? OTEXT("NCLOB") : OTEXT("CLOB"));
            break;
        }
        case SQLT_BLOB:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("BLOB"));
            break;
        }
        case SQLT_BFILE:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("BINARY FILE LOB"));
            break;
        }
        case SQLT_CFILE:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("CFILE"));
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_TIMESTAMP:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("TIMESTAMP(%i)"), (int) col->prec);
            break;
        }
        case SQLT_TIMESTAMP_TZ:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("TIMESTAMP(%i) WITH TIME ZONE"), (int) col->prec);
            break;
        }
        case SQLT_TIMESTAMP_LTZ:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("TIMESTAMP(%i) WITH LOCAL TIME ZONE"), (int) col->prec);
            break;
        }
        case SQLT_INTERVAL_YM:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("INTERVAL(%i) YEAR TO MONTH(%i)"),
                                  (int) col->prec, (int) col->prec2);
            break;
        }
        case SQLT_INTERVAL_DS:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("INTERVAL(%i) DAY TO SECOND(%i)"),
                                  (int) col->prec, (int) col->prec2);
            break;
        }

    #endif

        case SQLT_REF:
        {
            RETVAL = osprintf(buffer, (int)len,  OTEXT("REF"));
            break;
        }

    #if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_PNTY:

    #endif

        case SQLT_NTY:
        {
            if (col->typinf)
            {
                otext fullname[(OCI_SIZE_OBJ_NAME * 2) + 2] = OTEXT("");
                StringGetFullTypeName(col->typinf->schema, NULL, col->typinf->name, NULL, fullname, (sizeof(fullname) / sizeof(otext)) - 1);
                RETVAL = osprintf(buffer, (int)len, fullname);
            }
            else
            {
                RETVAL = osprintf(buffer, (int)len, OTEXT("NAMED TYPE"));
            }
            break;
        }


#if OCI_VERSION_COMPILE >= OCI_12_1

        case SQLT_BOL:
        {
            RETVAL = osprintf(buffer, (int)len, OTEXT("PL/SQL BOOLEAN"));
            break;
        }

        case SQLT_REC:
        case SQLT_UNDOCUMENTED_REC:
        {
            RETVAL = osprintf(buffer, (int)len, col->typinf ? col->typinf->name : OTEXT("PL/SQL RECORD"));
            break;
        }

        case SQLT_TAB:
        {
            RETVAL = osprintf(buffer, (int)len, col->typinf ? col->typinf->name : OTEXT("PL/SQL TABLE INDEX BY"));
            break;
        }

        case SQLT_UNDOCUMENTED_BIN_INTEGER:
        {
            RETVAL = osprintf(buffer, (int)len, OTEXT("PL/SQL BINARY INTEGER"));
            break;
        }

        case OCI_TYPECODE_PLS_INTEGER:
        {
            RETVAL = osprintf(buffer, (int)len, OTEXT("PL/SQL INTEGER"));
            break;
        }

#endif

        default:
        {
            RETVAL = osprintf(buffer, (int)len, OTEXT("?"));
            break;
        }
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * ColumnGetTypeInfo
(
    OCI_Column *col
)
{
    GET_PROP(OCI_TypeInfo*, NULL, OCI_IPC_COLUMN, col, typinf, NULL, NULL, NULL)
}

/* --------------------------------------------------------------------------------------------- *
 * ColumnGetSubType
 * --------------------------------------------------------------------------------------------- */

unsigned int ColumnGetSubType
(
    OCI_Column *col
)
{
    CALL_ENTER(unsigned int, OCI_UNKNOWN)
    CALL_CHECK_PTR(OCI_IPC_COLUMN, col)

    if (OCI_CDT_LONG      == col->datatype  ||
        OCI_CDT_LOB       == col->datatype  ||
        OCI_CDT_FILE      == col->datatype  ||
        OCI_CDT_TIMESTAMP == col->datatype  ||
        OCI_CDT_INTERVAL  == col->datatype  ||
        OCI_CDT_NUMERIC   == col->datatype)
    {
        RETVAL = col->subtype;
    }

    CALL_EXIT()
}

