/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2021 Vincent ROGIER <vince.rogier@ocilib.net>
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
#include "stringutils.h"
#include "typeinfo.h"

/* --------------------------------------------------------------------------------------------- *
* OcilibColumnGetAttrInfo
* --------------------------------------------------------------------------------------------- */

boolean OcilibColumnGetAttributeInfo
(
    OCI_Column  *col,
    unsigned int count,
    unsigned int index,
    size_t      *p_size,
    size_t      *p_align
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLUMN, col
    )

    CHECK_PTR(OCI_IPC_COLUMN, col)

    if (index >= count)
    {
        *p_size = 0;
        JUMP_EXIT()
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
            OcilibObjectGetUserStructSize(col->typinf, p_size, p_align);
            break;
        }
        default:
        {
            *p_size  = sizeof(void *);
            *p_align = *p_size;
            break;
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnRetrieveInfo
 * --------------------------------------------------------------------------------------------- */

boolean OcilibColumnRetrieveInfo
(
    OCI_Column     *col,
    OCI_Connection *con,
    OCI_Statement  *stmt,
    void           *handle,
    int             index,
    int             ptype
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ (stmt ? OCI_IPC_STATEMENT : OCI_IPC_CONNECTION), (stmt ? (void*) stmt : (void*) con)
    )

    void *param = NULL;

    CHECK_PTR(OCI_IPC_COLUMN, col)

    /* get descriptor */

    if (OCI_DESC_COLLECTION == ptype)
    {
        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_COLLECTION_ELEMENT,
            handle, &param, NULL,
            con->err
        )
    }
    else
    {
        const ub4 htype = (OCI_DESC_RESULTSET == ptype) ? OCI_HTYPE_STMT : OCI_DTYPE_PARAM;

        CHECK_OCI
        (
            con->err,
            OCIParamGet,
            (dvoid *) handle, htype,  con->err,
            (void**) &param, (ub4) index
        )
    }

    /* sql code */
    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_PARAM, OCI_ATTR_DATA_TYPE,
        param, &col->sqlcode, NULL,
        con->err
    )

    /* when the column is a record from a PL/SQL table, OCI returns an undocumented SQLT code */

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (SQLT_UNDOCUMENTED_REC == col->sqlcode)
    {
        col->sqlcode = SQLT_REC;
    }

#endif

    /* size */

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_PARAM, OCI_ATTR_DATA_SIZE,
        param, &col->size, NULL,
        con->err
    )

    /* scale */

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_PARAM, OCI_ATTR_SCALE,
        param, &col->scale, NULL,
        con->err
    )

    /* precision */

    if (OCI_DESC_RESULTSET == ptype)
    {
        sb2 prec = 0;

        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_PRECISION,
            param, &prec, NULL,
            con->err
        )

        col->prec = (sb2) prec;
    }
    else
    {
        ub1 prec = 0;

        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_PRECISION,
            param, &prec, NULL,
            con->err
        )

        col->prec = (sb2) prec;
    }

    /* charset form */

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_PARAM, OCI_ATTR_CHARSET_FORM,
        param, &col->csfrm, NULL,
        con->err
    )

    /* type of column length for string based column */

#if OCI_VERSION_COMPILE >= OCI_9_2

    if (OcilibConnectionIsVersionSupported(con, OCI_9_2))
    {
        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_CHAR_USED,
            param, &col->charused, NULL,
            con->err
        )
    }

    /* char size */

    if (col->charused)
    {
        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_CHAR_SIZE,
            param, &col->charsize, NULL,
            con->err
        )
    }

#endif

    if (OcilibConnectionIsVersionSupported(con, OCI_9_0))
    {
        /* fractional time precision for timestamps */

        if ( SQLT_TIMESTAMP     == col->sqlcode ||
             SQLT_TIMESTAMP_TZ  == col->sqlcode ||
             SQLT_TIMESTAMP_LTZ == col->sqlcode )
        {
            CHECK_ATTRIB_GET
            (
                OCI_DTYPE_PARAM, OCI_ATTR_FSPRECISION,
                param, &col->prec, NULL,
                con->err
            )
        }

        /* leading and fractional precision for interval */

        if (SQLT_INTERVAL_DS == col->sqlcode  || SQLT_INTERVAL_YM == col->sqlcode)
        {
            CHECK_ATTRIB_GET
            (
                OCI_DTYPE_PARAM, OCI_ATTR_LFPRECISION,
                param, &col->prec, NULL,
                con->err
            )

            CHECK_ATTRIB_GET
            (
                OCI_DTYPE_PARAM, OCI_ATTR_FSPRECISION,
                param, &col->prec2, NULL,
                con->err
            )
        }
    }

#if OCI_VERSION_COMPILE >= OCI_12_2

    if (OcilibConnectionIsVersionSupported(con, OCI_12_2))
    {
        if (OCI_DESC_RESULTSET == ptype)
        {
            CHECK_ATTRIB_GET
            (
                OCI_DTYPE_PARAM, OCI_ATTR_COLLATION_ID,
                param, &col->collation_id, NULL,
                con->err
            )
        }
    }

#endif

#if OCI_VERSION_COMPILE >= OCI_12_1

    if (OcilibConnectionIsVersionSupported(con, OCI_12_1))
    {
        if (ptype < OCI_DESC_TYPE)
        {
            ub8 value = 0;

            CHECK_ATTRIB_GET
            (
                OCI_DTYPE_PARAM, OCI_ATTR_COL_PROPERTIES,
                param, &value, NULL,
                con->err
            )

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
        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_IS_NULL,
            param, &col->nullable, NULL,
            con->err
        )
    }
    else
    {
        col->nullable = TRUE;
    }

    /* name */

#if defined(OCI_CHARSET_WIDE)

    /* Ugly workaround for Oracle Bug 9838993 */

    if ((OCI_DESC_RESULTSET == ptype) && (Env.env_vars[OCI_VARS_WORKAROUND_UTF16_COLUMN_NAME]))
    {
        OCIParamStruct *param_struct = (OCIParamStruct*) param;

        if (param_struct && param_struct->column_info && param_struct->column_info->name &&
            (param_struct->column_info->attributes[1] != 0))
        {
            const size_t char_count = param_struct->column_info->attributes[1];

            ALLOC_DATA(OCI_IPC_STRING, col->name, char_count + 1)

            OcilibStringAnsiToNative(param_struct->column_info->name, col->name, (int) char_count);
        }
        else
        {
            CHECK(FALSE)
        }
    }
    else

#endif

    {
        int       dbsize   = 0;
        dbtext   *dbstr    = NULL;
        const ub4 attrname = (OCI_DESC_COLLECTION == ptype) ? OCI_ATTR_TYPE_NAME : OCI_ATTR_NAME;

        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_PARAM, attrname,
            param, &dbstr, &dbsize,
            con->err
        )

        if (dbstr)
        {
            col->name = OcilibStringDuplicateFromDBString(dbstr, dbcharcount(dbsize));

            CHECK_NULL(col->name)
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
        dbtext *dbstr_name     = NULL;
        dbtext *dbstr_schema   = NULL;
        dbtext *dbstr_package  = NULL;
        int     dbsize_name    = 0;
        int     dbsize_schema  = 0;
        int     dbsize_package = 0;

        otext schema_name[OCI_SIZE_OBJ_NAME + 1]     = OTEXT("");
        otext package_name[OCI_SIZE_OBJ_NAME + 1]    = OTEXT("");
        otext type_name[OCI_SIZE_OBJ_NAME + 1]       = OTEXT("");
        otext full_name[(OCI_SIZE_OBJ_NAME * 3) + 3] = OTEXT("");

        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_TYPE_NAME,
            param, &dbstr_name, &dbsize_name,
            con->err
        )
        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_SCHEMA_NAME,
            param, &dbstr_schema, &dbsize_schema,
            con->err
        )

#if OCI_VERSION_COMPILE >= OCI_12_1

        if (SQLT_REC == col->sqlcode || SQLT_TAB == col->sqlcode)
        {
            CHECK_ATTRIB_GET
            (
                OCI_DTYPE_PARAM, OCI_ATTR_PACKAGE_NAME,
                handle, &dbstr_package, &dbsize_package,
                con->err
            )
        }

#endif

        /* Retrieve correct schema name */

        if (NULL != dbstr_schema && (dbsize_schema > 0))
        {
            OcilibStringOracleToNative(dbstr_schema, schema_name, dbcharcount(dbsize_schema));

            if (0 == OcilibStringCaseCompare(schema_name, OTEXT("PUBLIC")))
            {
                schema_name[0] = 0;
            }
        }

        /* Retrieve correct type name */

        if (NULL != dbstr_package && (dbsize_package > 0))
        {
            OcilibStringOracleToNative(dbstr_package, package_name, dbcharcount(dbsize_package));
        }

        /* Retrieve correct type name */

        if (NULL != dbstr_name && (dbsize_name > 0))
        {
            OcilibStringOracleToNative(dbstr_name, type_name, dbcharcount(dbsize_name));
        }

        /* Format full type name respecting case sensitivity if needed in order to not fail type
         * info retrieval.*/

        OcilibStringGetFullTypeName(schema_name, package_name, type_name, NULL, full_name,
                              (sizeof(full_name) / sizeof(otext)) - 1);

        col->typinf = OcilibTypeInfoGet(con, full_name, OCI_TIF_TYPE);
        CHECK_NULL(col->typinf);
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != param)
        {
            OCIDescriptorFree(param, OCI_DTYPE_PARAM);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnMapInfo
 * --------------------------------------------------------------------------------------------- */

boolean OcilibColumnMapInfo
(
    OCI_Column    *col,
    OCI_Statement *stmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLUMN, col
    )

    const ub2 char_size = (ub2) ( (Env.nls_utf8 ) ? OCI_UTF8_BYTES_PER_CHAR : sizeof(otext) );

    CHECK_PTR(OCI_IPC_COLUMN, col)

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
            col->libcode    = SQLT_RDD;
            col->datatype   = OCI_CDT_TEXT;
            col->bufsize    = (ub4)sizeof(OCIRowid*);
            col->handletype = OCI_DTYPE_ROWID;
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

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibColumnGetName
(
    OCI_Column *col
)
{
    GET_PROP
    (
        /* result */ const otext *, NULL,
        /* handle */ OCI_IPC_COLUMN, col,
        /* member */ name
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibColumnGetType
(
    OCI_Column *col
)
{
    GET_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* handle */ OCI_IPC_COLUMN, col,
        /* member */ datatype
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetCharsetForm
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibColumnGetCharsetForm
(
    OCI_Column *col
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_CSF_NONE,
        /* context */ OCI_IPC_COLUMN, col
    )

    CHECK_PTR(OCI_IPC_COLUMN, col)

    unsigned int charset_form = OCI_CSF_NONE;

    if (SQLCS_NCHAR == col->csfrm)
    {
        charset_form = OCI_CSF_NATIONAL;
    }
    else if (SQLCS_IMPLICIT == col->csfrm)
    {
        charset_form = OCI_CSF_DEFAULT;
    }

    SET_RETVAL(charset_form)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibColumnGetSize
(
    OCI_Column *col
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_COLUMN, col
    )

    CHECK_PTR(OCI_IPC_COLUMN, col)

    /* Oracle 9i introduced CHAR attribute on string columns to indicate the
       size of the column is not in bytes (default) but in chars
       OCI_ColumnDescribe() already managed the Oracle compatibility
       version, so if the column character size member is zero it means :
       - the column is not a string column
       - the size is not in char
       - client does not support the OCI_ATTR_CHAR_SIZE attribute */

    SET_RETVAL((col->charused && col->charsize > 0) ? col->charsize : col->size)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetScale
 * --------------------------------------------------------------------------------------------- */

int OcilibColumnGetScale
(
    OCI_Column *col
)
{
    GET_PROP
    (
        /* result */ int, 0,
        /* handle */ OCI_IPC_COLUMN, col,
        /* member */ scale
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetPrecision
 * --------------------------------------------------------------------------------------------- */

int OcilibColumnGetPrecision
(
    OCI_Column *col
)
{
    ENTER_FUNC
    (
        /* returns */ int, 0,
        /* context */ OCI_IPC_COLUMN, col
    )

    CHECK_PTR(OCI_IPC_COLUMN, col)

    SET_RETVAL((OCI_CDT_NUMERIC == col->datatype) ? (int) col->prec : 0)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetFractionalPrecision
 * --------------------------------------------------------------------------------------------- */

int OcilibColumnGetFractionalPrecision
(
    OCI_Column *col
)
{
    ENTER_FUNC
    (
        /* returns */ int, 0,
        /* context */ OCI_IPC_COLUMN, col
    )

    CHECK_PTR(OCI_IPC_COLUMN, col)

    int precision = 0;

    if (OCI_CDT_TIMESTAMP == col->datatype)
    {
        precision = (int) col->prec;
    }
    else if (OCI_CDT_INTERVAL == col->datatype)
    {
        precision = (int)col->prec2;
    }

    SET_RETVAL(precision)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetLeadingPrecision
 * --------------------------------------------------------------------------------------------- */

int OcilibColumnGetLeadingPrecision
(
    OCI_Column *col
)
{
    ENTER_FUNC
    (
        /* returns */ int, 0,
        /* context */ OCI_IPC_COLUMN, col
    )

    CHECK_PTR(OCI_IPC_COLUMN, col)

    SET_RETVAL((OCI_CDT_INTERVAL == col->datatype) ? (int) col->prec : 0)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetNullable
 * --------------------------------------------------------------------------------------------- */

boolean OcilibColumnGetNullable
(
    OCI_Column *col
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLUMN, col
    )

    CHECK_PTR(OCI_IPC_COLUMN, col)

    SET_RETVAL(1 == col->nullable)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetCharUsed
 * --------------------------------------------------------------------------------------------- */

boolean OcilibColumnGetCharUsed
(
    OCI_Column *col
)
{
    GET_PROP
    (
        /* result */ boolean, FALSE,
        /* handle */ OCI_IPC_COLUMN, col,
        /* member */ charused
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetPropertyFlags
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibColumnGetPropertyFlags
(
    OCI_Column *col
)
{
    GET_PROP
    (
        /* result */ unsigned int, 0,
        /* handle */ OCI_IPC_COLUMN, col,
        /* member */ props
    )
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ColumnGetCollationID
* --------------------------------------------------------------------------------------------- */

unsigned int OcilibColumnGetCollationID
(
    OCI_Column *col
)
{
    GET_PROP
    (
        /* result */ unsigned int, OCI_CCI_NONE,
        /* handle */ OCI_IPC_COLUMN, col,
        /* member */ collation_id
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetSQLType
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibColumnGetSqlType
(
    OCI_Column *col
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_COLUMN, col
    )

    CHECK_PTR(OCI_IPC_COLUMN, col)

    const otext *type = NULL;

    /* VARCHAR type will not be returned because Oracle does not make any
       difference with VARCHAR2. If a column is created with VARCHAR, it is
       internally created as VARCHAR2
    */

    switch(col->sqlcode)
    {
        case SQLT_AFC:
        {
            type = (SQLCS_NCHAR == col->csfrm) ? OTEXT("NCHAR") : OTEXT("CHAR");;
            break;
        }
        case SQLT_AVC:
        case SQLT_STR:
        case SQLT_CHR:
        {
            type = (SQLCS_NCHAR == col->csfrm) ? OTEXT("NVARCHAR2") : OTEXT("VARCHAR2");
            break;
        }
        case SQLT_NUM:
        {
            type = (SCALE_FLOAT == col->scale && col->prec > 0) ? OTEXT("FLOAT") : OTEXT("NUMBER");
            break;
        }
        case SQLT_INT:
        {
            type = OTEXT("INTEGER");
            break;
        }
        case SQLT_FLT:
        {
            type = OTEXT("FLOAT");
            break;
        }

#if OCI_VERSION_COMPILE >= OCI_10_1

        case SQLT_BFLOAT:
        case SQLT_IBFLOAT:
        {
            type = OTEXT("BINARY FLOAT");
            break;
        }
        case SQLT_BDOUBLE:
        case SQLT_IBDOUBLE:
        {
            type = OTEXT("BINARY DOUBLE");
            break;
        }

#endif

        case SQLT_LNG:
        {
            type = OTEXT("LONG");
            break;
        }
        case SQLT_DAT:
        case SQLT_ODT:
        case SQLT_DATE:
        {
            type = OTEXT("DATE");
            break;
        }
        case SQLT_RDD:
        case SQLT_RID:
        {
            type = OTEXT("ROWID");
            break;
        }
        case SQLT_BIN:
        {
            type = OTEXT("RAW");
            break;
        }
        case SQLT_LBI:
        {
            type = OTEXT("LONG RAW");
            break;
        }
        case SQLT_RSET:
        {
            type = OTEXT("RESULTSET");
            break;
        }
        case SQLT_CUR:
        {
            type = OTEXT("CURSOR");
            break;
        }
        case SQLT_CLOB:
        {
            type = (col->subtype == OCI_NCLOB) ? OTEXT("NCLOB") : OTEXT("CLOB");
            break;
        }
        case SQLT_BLOB:
        {
            type = OTEXT("BLOB");
            break;
        }
        case SQLT_BFILE:
        {
            type = OTEXT("BINARY FILE LOB");
            break;
        }
        case SQLT_CFILE:
        {
            type = OTEXT("CFILE");
            break;
        }

#if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_TIMESTAMP:
        {
            type = OTEXT("TIMESTAMP");
            break;
        }
        case SQLT_TIMESTAMP_TZ:
        {
            type = OTEXT("TIMESTAMP WITH TIME ZONE");
            break;
        }
        case SQLT_TIMESTAMP_LTZ:
        {
            type = OTEXT("TIMESTAMP WITH LOCAL TIME ZONE");
            break;
        }
        case SQLT_INTERVAL_YM:
        {
            type = OTEXT("INTERVAL YEAR TO MONTH");
            break;
        }
        case SQLT_INTERVAL_DS:
        {
            type = OTEXT("INTERVAL DAY TO SECOND");
            break;
        }

#endif

        case SQLT_REF:
        {
            type = OTEXT("REF");
            break;
        }

#if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_PNTY:

#endif

        case SQLT_NTY:
        {
            type = col->typinf ? col->typinf->name : OTEXT("NAMED TYPE");
            break;
        }

#if OCI_VERSION_COMPILE >= OCI_12_1

        case SQLT_BOL:
        {
            type = OTEXT("PL/SQL BOOLEAN");
            break;
        }

        case SQLT_REC:
        case SQLT_UNDOCUMENTED_REC:
        {
            type = col->typinf ? col->typinf->name : OTEXT("PL/SQL RECORD");
            break;
        }

        case SQLT_TAB:
        {
            type = col->typinf ? col->typinf->name : OTEXT("PL/SQL TABLE INDEX BY");
            break;
        }

        case SQLT_UNDOCUMENTED_BIN_INTEGER:
        {
            type = OTEXT("PL/SQL BINARY INTEGER");
            break;
        }

        case OCI_TYPECODE_PLS_INTEGER:
        {
            type = OTEXT("PL/SQL INTEGER");
            break;
        }

#endif

        default:
        {
            /* unknown data type ? Should not happen because all
                 data types are supported */

            type = OTEXT("?");
        }
    }

    SET_RETVAL(type)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetFullSQLType
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibColumnGetFullSqlType
(
    OCI_Column  *col,
    otext       *buffer,
    unsigned int len
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_COLUMN, col
    )

    CHECK_PTR(OCI_IPC_COLUMN, col)
    CHECK_PTR(OCI_IPC_STRING, buffer)

    buffer[0] = 0;

    unsigned int size = 0;

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
            size = OcilibStringFormat(buffer, (int)len, OTEXT("%lsCHAR(%i%ls)"),
#else
            size = OcilibStringFormat(buffer, (int)len, OTEXT("%sCHAR(%i%s)"),
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

            size = OcilibStringFormat(buffer, (int)len, fmt, SQLCS_NCHAR == col->csfrm ? OTEXT("N") : OTEXT(""),
                                      (int) (col->charused ? col->charsize : col->size),
                                      col->charused &&  SQLCS_NCHAR != col->csfrm ? OTEXT(" CHAR") : OTEXT(""));
            break;
        }
        case SQLT_NUM:
        {
            if (SCALE_FLOAT == col->scale && col->prec > 0)
            {
                size = OcilibStringFormat(buffer, (int)len,  OTEXT("FLOAT(%i)"), col->prec);
            }
            else if (col->scale > 0 && col->prec > 0)
            {
                size = OcilibStringFormat(buffer, (int)len,  OTEXT("NUMBER(%i,%i)"), (int) col->prec, (int) col->scale);
            }
            else if (col->prec > 0)
            {
                size = OcilibStringFormat(buffer, (int)len,  OTEXT("NUMBER(%i)"), (int) col->prec);
            }
            else
            {
                size = OcilibStringFormat(buffer, (int)len,  OTEXT("NUMBER"));
            }

            break;
        }
        case SQLT_INT:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("NUMBER"));
            break;
        }
        case SQLT_FLT:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("FLOAT(%i)"), (int) col->prec);
            break;
        }

#if OCI_VERSION_COMPILE >= OCI_10_1

        case SQLT_BFLOAT:
        case SQLT_IBFLOAT:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("BINARY FLOAT"));
            break;
        }
        case SQLT_BDOUBLE:
        case SQLT_IBDOUBLE:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("BINARY DOUBLE"));
            break;
        }

#endif

        case SQLT_LNG:
        {
            size = OcilibStringFormat(buffer, (int)len, OTEXT("LONG"));
            break;
        }
        case SQLT_DAT:
        case SQLT_ODT:
        case SQLT_DATE:
        {
            size = OcilibStringFormat(buffer, (int)len, OTEXT("DATE"));
            break;
        }
        case SQLT_RDD:
        case SQLT_RID:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("ROWID"));
            break;
        }
        case SQLT_BIN:
        {
            size = OcilibStringFormat(buffer, (int)len, OTEXT("RAW(%i)"), (int) col->size);
            break;
        }
        case SQLT_LBI:
        {
            size = OcilibStringFormat(buffer, (int)len, OTEXT("LONG RAW(%i)"), (int) col->size);
            break;
        }
        case SQLT_RSET:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("RESULTSET"));
            break;
        }
        case SQLT_CUR:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("CURSOR"));
            break;
        }
        case SQLT_CLOB:
        {
            size = OcilibStringFormat(buffer, (int)len, (col->subtype == OCI_NCLOB) ? OTEXT("NCLOB") : OTEXT("CLOB"));
            break;
        }
        case SQLT_BLOB:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("BLOB"));
            break;
        }
        case SQLT_BFILE:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("BINARY FILE LOB"));
            break;
        }
        case SQLT_CFILE:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("CFILE"));
            break;
        }

#if OCI_VERSION_COMPILE >= OCI_9_0

        case SQLT_TIMESTAMP:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("TIMESTAMP(%i)"), (int) col->prec);
            break;
        }
        case SQLT_TIMESTAMP_TZ:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("TIMESTAMP(%i) WITH TIME ZONE"), (int) col->prec);
            break;
        }
        case SQLT_TIMESTAMP_LTZ:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("TIMESTAMP(%i) WITH LOCAL TIME ZONE"), (int) col->prec);
            break;
        }
        case SQLT_INTERVAL_YM:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("INTERVAL(%i) YEAR TO MONTH(%i)"),
                                      (int) col->prec, (int) col->prec2);
            break;
        }
        case SQLT_INTERVAL_DS:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("INTERVAL(%i) DAY TO SECOND(%i)"),
                                      (int) col->prec, (int) col->prec2);
            break;
        }

#endif

        case SQLT_REF:
        {
            size = OcilibStringFormat(buffer, (int)len,  OTEXT("REF"));
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

                OcilibStringGetFullTypeName(col->typinf->schema, NULL, col->typinf->name, NULL,
                                            fullname, (sizeof(fullname) / sizeof(otext)) - 1);

                size = OcilibStringFormat(buffer, (int)len, fullname);
            }
            else
            {
                size = OcilibStringFormat(buffer, (int)len, OTEXT("NAMED TYPE"));
            }
            break;
        }

#if OCI_VERSION_COMPILE >= OCI_12_1

        case SQLT_BOL:
        {
            size = OcilibStringFormat(buffer, (int)len, OTEXT("PL/SQL BOOLEAN"));
            break;
        }

        case SQLT_REC:
        case SQLT_UNDOCUMENTED_REC:
        {
            size = OcilibStringFormat(buffer, (int)len, col->typinf ? col->typinf->name : OTEXT("PL/SQL RECORD"));
            break;
        }

        case SQLT_TAB:
        {
            size = OcilibStringFormat(buffer, (int)len, col->typinf ? col->typinf->name : OTEXT("PL/SQL TABLE INDEX BY"));
            break;
        }

        case SQLT_UNDOCUMENTED_BIN_INTEGER:
        {
            size = OcilibStringFormat(buffer, (int)len, OTEXT("PL/SQL BINARY INTEGER"));
            break;
        }

        case OCI_TYPECODE_PLS_INTEGER:
        {
            size = OcilibStringFormat(buffer, (int)len, OTEXT("PL/SQL INTEGER"));
            break;
        }

#endif

        default:
        {
            size = OcilibStringFormat(buffer, (int)len, OTEXT("?"));
            break;
        }
    }

    SET_RETVAL(size)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OcilibColumnGetTypeInfo
(
    OCI_Column *col
)
{
    GET_PROP
    (
        /* result */ OCI_TypeInfo*, NULL,
        /* handle */ OCI_IPC_COLUMN, col,
        /* member */ typinf
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibColumnGetSubType
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibColumnGetSubType
(
    OCI_Column *col
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_COLUMN, col
    )

    CHECK_PTR(OCI_IPC_COLUMN, col)

    int subtype =OCI_UNKNOWN;

    if (OCI_CDT_LONG      == col->datatype  ||
        OCI_CDT_LOB       == col->datatype  ||
        OCI_CDT_FILE      == col->datatype  ||
        OCI_CDT_TIMESTAMP == col->datatype  ||
        OCI_CDT_INTERVAL  == col->datatype  ||
        OCI_CDT_NUMERIC   == col->datatype)
    {
        subtype = col->subtype;
    }

    SET_RETVAL(subtype)

    EXIT_FUNC()
}
