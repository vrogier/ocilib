/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "typeinfo.h"

#include "column.h"
#include "list.h"
#include "macros.h"
#include "memory.h"
#include "stringutils.h"

static OCI_UInt32 TypeInfoTypeValues[] =
{
    OCI_TIF_TABLE,
    OCI_TIF_VIEW,
    OCI_TIF_TYPE
};

typedef struct TypeInfoFindParams
{
    OCI_TypeInfoType type;
    otext          * schema;
    otext          * name;
} TypeInfoFindParams;

/* --------------------------------------------------------------------------------------------- *
 * OcilibTypeInfoFind
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibTypeInfoFind(OCI_TypeInfo *typinf, TypeInfoFindParams *find_params)
{
    return
        typinf &&
        find_params &&
        typinf->type == find_params->type &&
        OcilibStringCaseCompare(typinf->name,   find_params->name) == 0 &&
        OcilibStringCaseCompare(typinf->schema, find_params->schema) == 0;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTypeInfoFindOrCreate
 * --------------------------------------------------------------------------------------------- */

static OCI_TypeInfo* OcilibTypeInfoFindOrCreate(OCI_Connection* con, TypeInfoFindParams* p_find_params, OCI_Boolean* p_is_created)
{
    OCI_TypeInfo* typinf = OcilibListFind(con->tinfs, (POCI_LIST_FIND) OcilibTypeInfoFind, p_find_params);

    *p_is_created = FALSE;

    if (NULL == typinf)
    {
        typinf = OcilibListAppend(con->tinfs, sizeof(OCI_TypeInfo));
        if (typinf == NULL)
        {
            return typinf;
        }

        typinf->con         = con;
        typinf->env         = con->env;
        typinf->htype       = OCI_IPC_TYPE_INFO;
        typinf->name        = OcilibStringDuplicate(typinf->con->env, p_find_params->name);
        typinf->schema      = OcilibStringDuplicate(typinf->con->env, p_find_params->schema);
        typinf->struct_size = 0;
        typinf->align       = 0;

        *p_is_created = TRUE;
    }

    return typinf;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTypeInfoDispose
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTypeInfoDispose
(
    OCI_TypeInfo *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);

    for (ub2 i = 0; i < typinf->nb_cols; i++)
    {
        FREE(typinf->cols[i].name);
    }

    FREE(typinf->cols);
    FREE(typinf->name);
    FREE(typinf->schema);
    FREE(typinf->offsets);

    OcilibErrorResetSource(NULL, typinf);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTypeInfoGet
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OcilibTypeInfoGet
(
    OCI_Connection  *con,
    const otext     *name,
    OCI_TypeInfoType type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_TypeInfo*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    TypeInfoFindParams find_params;

    OCI_TypeInfo *typinf     = NULL;
    OCI_TypeInfo *syn_typinf = NULL;
    OCIDescribe  *dschp      = NULL;
    OCIParam     *param_root = NULL;
    OCIParam     *param_type = NULL;
    OCIParam     *param_cols = NULL;
    OCIParam     *param_list = NULL;
    otext        *str        = NULL;
    OCI_Int32     ptype      = 0;
    ub1           desc_type  = 0;
    ub4           attr_type  = 0;
    ub4           num_type   = 0;

    otext* syn_schema_name = NULL;
    otext* syn_object_name = NULL;
    otext* syn_link_name   = NULL;

    OCI_UInt32 size_schema = 0;
    OCI_UInt32 size_object = 0;
    OCI_UInt32 size_link   = 0;

    otext* sp_schema_name = NULL;
    otext* sp_object_name = NULL;

    OCI_UInt32 sp_size_schema = 0;
    OCI_UInt32 sp_size_object = 0;

    otext buffer[(OCI_SIZE_OBJ_NAME * 2) + 2] = OTEXT("");

    OCI_Size  max_chars = sizeof(buffer) / sizeof(otext) - 1;
    dbtext  * dbstr1    = NULL;
    OCI_Int32 dbsize1   = -1;
    dbtext  * dbstr2    = NULL;
    OCI_Int32 dbsize2   = -1;
    dbtext  * dbstr3    = NULL;
    OCI_Int32 dbsize3   = -1;
    sb4       pbsp      = 1;

    otext obj_schema[OCI_SIZE_OBJ_NAME + 1];
    otext obj_name[OCI_SIZE_OBJ_NAME + 1];

    OCI_Boolean is_created = FALSE;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_PTR(OCI_IPC_STRING,     name);
    CHECK_ENUM_VALUE(type, TypeInfoTypeValues, OTEXT("Type"));

    obj_schema[0] = 0;
    obj_name[0]   = 0;

    /* is the schema provided in the object name ? */

    for (str = (otext *) name; *str; str++)
    {
        if (*str == OTEXT('.'))
        {
            ostrncat(obj_schema, name,  (OCI_Size)(str - name));
            ostrncat(obj_name,   ++str, (OCI_Size) OCI_SIZE_OBJ_NAME);
            break;
        }
    }

    /* if the schema is not provided, we just copy the object name */

    if (!obj_name[0])
    {
        ostrncat(obj_name, name, (OCI_Size) OCI_SIZE_OBJ_NAME);
    }

    /* type name must be uppercase if not quoted */

    if (obj_name[0] != OTEXT('"'))
    {
        for (str = obj_name; *str; str++)
        {
            *str = (otext)otoupper(*str);
        }
    }

    /* schema name must be uppercase if not quoted */

    if (obj_schema[0] != OTEXT('"'))
    {
        for (str = obj_schema; *str; str++)
        {
            *str = (otext)otoupper(*str);
        }
    }

    /* first try to find it in list */

    find_params.type   = type;
    find_params.name   = obj_name;
    find_params.schema = obj_schema;

    LIST_ATOMIC_OPERATION
    (
        con->tinfs,
        {
            typinf = OcilibTypeInfoFindOrCreate(con, &find_params, &is_created);
        }
    );

    CHECK_NULL(typinf);

    /* Not found, so create type object */

    if (is_created)
    {
        /* allocate describe handle */

        CHECK(OcilibMemoryAllocHandle(typinf->con->env, typinf->con->env->handle, (dvoid **)(void *)&dschp, OCI_HTYPE_DESCRIBE));

        /* perform describe */

        str = buffer;

        /* compute full object name */

        if (IS_STRING_VALID(typinf->schema))
        {
            str        = ostrncat(buffer, typinf->schema, max_chars);
            max_chars -= ostrlen(typinf->schema);

            str        = ostrncat(str, OTEXT("."), max_chars);
            max_chars -= (OCI_Size)1;
        }

        ostrncat(str, typinf->name, max_chars);

        dbstr1 = OcilibStringGetDBString(typinf->con->env, str, &dbsize1);

        /* set public scope to include synonyms */

        CHECK_ATTRIB_SET
        (
            OCI_HTYPE_DESCRIBE, OCI_ATTR_DESC_PUBLIC,
            dschp, &pbsp, sizeof(pbsp),
            con->err
        );

        /* describe call */

        dvoid *describe_obj  = (dvoid*) dbstr1;
        ub4    describe_size = (ub4) dbsize1;
        ub1    describe_type = OCI_OTYPE_NAME;

        if (OCI_TIF_TYPE == type)
        {
            OCIType* tdo = NULL;
            sword    ret = OCI_ERROR;

            /* for types, as OCIDescribeAny() does not support some cases like SYS.RAW anymore
               like on previous Oracle versions, let's use OCITypeByName()
            */

            /* OCITypeByName() does not support package level types (PL/SQL records and
               PL/SQL index by tables). When it fails, fall back to describing the object */

            dbstr2 = OcilibStringGetDBString(typinf->con->env, typinf->schema, &dbsize2);
            dbstr3 = OcilibStringGetDBString(typinf->con->env, typinf->name, &dbsize3);

            OCI_CALL
            (
                ret,
                OCITypeByName,
                con->env->handle, con->err, con->handle,
                (CONST text*) dbstr2, (ub4) dbsize2,
                (CONST text*) dbstr3, (ub4) dbsize3,
                (text*)0, 0,
                OCI_DURATION_SESSION, OCI_TYPEGET_ALL,
                &tdo
            );

            if (OCI_SUCCESSFUL(ret))
            {
                describe_obj  = tdo;
                describe_size = 0;
                describe_type = OCI_OTYPE_PTR;
            }
        }

        CHECK_OCI
        (
            con->err,
            OCIDescribeAny,
            con->handle, con->err, describe_obj,
            describe_size, describe_type,
            OCI_DEFAULT, OCI_PTYPE_UNK, dschp
        );

        /* get parameter handle */

        CHECK_ATTRIB_GET
        (
            OCI_HTYPE_DESCRIBE, OCI_ATTR_PARAM,
            dschp, &param_root, NULL,
            con->err
        );

        /* get describe type */

        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_PARAM, OCI_ATTR_PTYPE,
            param_root, &desc_type, NULL,
            con->err
        );

        /* on successful describe call, retrieve all information about the object
           if it is not a synonym */

        switch (desc_type)
        {
            case OCI_PTYPE_TYPE:
            case OCI_PTYPE_LIST:
            {
                ub1     pdt = 0; /* Is Predefined Type */
                ub1     ist = 0; /* Is Sub Type */
                OCIRef* ref = NULL;

                if (OCI_TIF_TYPE != type)
                {
                    THROW(OcilibExceptionTypeInfoWrongType, name);
                }

                typinf->type = OCI_TIF_TYPE;

                if (OCI_PTYPE_LIST == desc_type)
                {
                    CHECK_OCI
                    (
                        con->err,
                        OCIParamGet,
                        (dvoid*)param_root, OCI_DTYPE_PARAM,
                        con->err, (void**)&param_type, (ub4) 0
                    );
                }
                else
                {
                    param_type = param_root;
                }

                /* get the object type descriptor */

                CHECK_ATTRIB_GET
                (
                    OCI_DTYPE_PARAM, OCI_ATTR_REF_TDO,
                    param_type, &ref, NULL,
                    con->err
                );

                CHECK_OCI
                (
                    con->err,
                    OCITypeByRef,
                    typinf->con->env->handle, con->err, ref,
                    OCI_DURATION_SESSION, OCI_TYPEGET_ALL,
                    &typinf->handle
                );

                /* check if the type is not final, e.g. can be inherited */

                CHECK_ATTRIB_GET
                (
                    OCI_DTYPE_PARAM, OCI_ATTR_IS_FINAL_TYPE,
                    param_type, &typinf->is_final, NULL,
                    con->err
                );

                /* check if it's system predefined type if order to avoid the next call that
                 * is not allowed on system types */

                CHECK_ATTRIB_GET
                (
                    OCI_DTYPE_PARAM, OCI_ATTR_IS_PREDEFINED_TYPE,
                    param_type, &pdt, NULL, con->err
                );

                if (!pdt)
                {
                    CHECK_ATTRIB_GET
                    (
                        OCI_DTYPE_PARAM, OCI_ATTR_TYPECODE,
                        param_type, &typinf->typecode, NULL,
                        con->err
                    );
                }

                /* check if the type is a subtype derived from a super type */

                CHECK_ATTRIB_GET
                (
                    OCI_DTYPE_PARAM, OCI_ATTR_IS_SUBTYPE,
                    param_type, &ist, NULL,
                    con->err
                );

                if (ist)
                {
                    otext sp_fullname[(OCI_SIZE_OBJ_NAME * 2) + 2] = OTEXT("");

                    /* get super type schema and name */

                    CHECK(OcilibStringGetAttribute(con, param_root, OCI_DTYPE_PARAM,
                                                   OCI_ATTR_SUPERTYPE_SCHEMA_NAME,
                                                   &sp_schema_name, &sp_size_schema));

                    CHECK(OcilibStringGetAttribute(con, param_root, OCI_DTYPE_PARAM,
                                                   OCI_ATTR_SUPERTYPE_NAME,
                                                   &sp_object_name, &sp_size_object));

                    /* compute super type full name */

                    OcilibStringGetFullTypeName(sp_schema_name, NULL, sp_object_name,
                                                NULL, sp_fullname,
                                                (sizeof(sp_fullname) / sizeof(otext)) - 1);

                    /* retrieve the type info of the real object */

                    typinf->parent_type = OcilibTypeInfoGet(con, sp_fullname, type);

                    CHECK_NULL(typinf->parent_type);
                }

                switch (typinf->typecode)
                {
                    case SQLT_OPAQUE_TYPE:
                    {
                        if (!IS_XMLTYPE(typinf))
                        {
                            THROW(OcilibExceptionDatatypeNotSupported, typinf->typecode);
                        }
                        break;
                    }
                    case SQLT_NTY:
#if OCI_VERSION_COMPILE >= OCI_12_1
                    case SQLT_REC:
#endif
                    {
                        param_list = param_type;
                        ptype      = OCI_DESC_TYPE;
                        attr_type  = OCI_ATTR_LIST_TYPE_ATTRS;
                        num_type   = OCI_ATTR_NUM_TYPE_ATTRS;
                        break;
                    }
                    case SQLT_NCO:
                    {
                        typinf->nb_cols = 1;

                        ptype      = OCI_DESC_COLLECTION;
                        param_cols = param_type;

                        CHECK_ATTRIB_GET
                        (
                            OCI_DTYPE_PARAM, OCI_ATTR_COLLECTION_TYPECODE,
                            param_type, &typinf->colcode, NULL,
                            con->err
                        );
                        break;
                    }
                    default:
                    {
                        if (!pdt)
                        {
                            THROW(OcilibExceptionDatatypeNotSupported, typinf->typecode);
                        }
                        break;
                    }
                }

                break;
            }
            case OCI_PTYPE_TABLE:
            case OCI_PTYPE_VIEW:
#if OCI_VERSION_COMPILE >= OCI_10_1
            case OCI_PTYPE_TABLE_ALIAS:
#endif
            {
                CHECK(((OCI_TIF_TABLE == type) && (OCI_PTYPE_VIEW != desc_type)) ||
                      ((OCI_TIF_VIEW == type) && (OCI_PTYPE_VIEW == desc_type)));

                typinf->type = (OCI_PTYPE_VIEW == desc_type ? OCI_TIF_VIEW : OCI_TIF_TABLE);
                attr_type    = OCI_ATTR_LIST_COLUMNS;
                num_type     = OCI_ATTR_NUM_COLS;
                ptype        = OCI_DESC_TABLE;
                param_list   = param_root;

                break;
            }
            case OCI_PTYPE_SYN:
            {
                otext syn_fullname[(OCI_SIZE_OBJ_NAME * 3) + 3] = OTEXT("");

                /* get link schema, object and database link names */

                CHECK(OcilibStringGetAttribute(con, param_root,
                                               OCI_DTYPE_PARAM,
                                               OCI_ATTR_SCHEMA_NAME,
                                               &syn_schema_name,
                                               &size_schema));

                CHECK(OcilibStringGetAttribute(con, param_root,
                                               OCI_DTYPE_PARAM,
                                               OCI_ATTR_NAME,
                                               &syn_object_name,
                                               &size_object));

                CHECK(OcilibStringGetAttribute(con, param_root,
                                               OCI_DTYPE_PARAM,
                                               OCI_ATTR_LINK,
                                               &syn_link_name,
                                               &size_link));

                /* compute link full name */

                OcilibStringGetFullTypeName(syn_schema_name, NULL,
                                            syn_object_name, syn_link_name,
                                            syn_fullname,
                                            (sizeof(syn_fullname) / sizeof(otext)) - 1);

                /* retrieve the type info of the real object */

                syn_typinf = OcilibTypeInfoGet(con, syn_fullname, type);

                /* do we have a valid object ? */

                CHECK_NULL(syn_typinf);

                break;
            }
        }

        /*  did we handle a supported object other than a synonym */

        if (OCI_UNKNOWN != ptype)
        {
            /* retrieve the columns parameter if not already retrieved */
            if (param_list)
            {
                CHECK_ATTRIB_GET
                (
                    OCI_DTYPE_PARAM, attr_type,
                    param_list, &param_cols, NULL,
                    con->err
                );

                CHECK_ATTRIB_GET
                (
                    OCI_DTYPE_PARAM, num_type,
                    param_list, &typinf->nb_cols, NULL,
                    con->err
                );
            }

            /* allocates memory for cached offsets */

            if (typinf->nb_cols > 0)
            {
                ALLOC_DATA(OCI_IPC_ARRAY, typinf->offsets, typinf->nb_cols);

                memset(typinf->offsets, -1, sizeof(*typinf->offsets) * typinf->nb_cols);
            }

            /* allocates memory for children */

            if (typinf->nb_cols > 0)
            {
                ALLOC_DATA(OCI_IPC_COLUMN, typinf->cols, typinf->nb_cols);

                /* describe children */

                for (ub2 i = 0; i < typinf->nb_cols; i++)
                {
                    CHECK(OcilibColumnRetrieveInfo(&typinf->cols[i], con, NULL, param_cols, i + 1, ptype));
                    CHECK(OcilibColumnMapInfo(&typinf->cols[i], typinf->con, NULL));
                }
            }
        }

        /* free describe handle */

        OcilibMemoryFreeHandle(typinf->con->env, dschp, OCI_HTYPE_DESCRIBE);
    }

    /* type checking sanity checks */

    if ((type != OCI_UNKNOWN) && ((syn_typinf && syn_typinf->type != type) || (!syn_typinf && typinf->type != type)))
    {
        THROW(OcilibExceptionTypeInfoWrongType, name);
    }

    SET_RETVAL(syn_typinf ? syn_typinf : typinf);

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(con->env, dbstr1);
        OcilibStringReleaseDBString(con->env, dbstr2);
        OcilibStringReleaseDBString(con->env, dbstr3);

        /* free temporary strings */

        OcilibMemoryFree(typinf->con->env, syn_link_name);
        OcilibMemoryFree(typinf->con->env, syn_object_name);
        OcilibMemoryFree(typinf->con->env, syn_schema_name);
        OcilibMemoryFree(typinf->con->env, sp_schema_name);
        OcilibMemoryFree(typinf->con->env, sp_object_name);

        OcilibMemoryFreeHandle(typinf->con->env, dschp, OCI_HTYPE_DESCRIBE);

        if (NULL != typinf && (FAILURE || NULL != syn_typinf))
        {
            LIST_ATOMIC_REMOVE(con->tinfs, typinf, OcilibTypeInfoDispose);

            FREE(typinf);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTypeInfoFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTypeInfoFree
(
    OCI_TypeInfo *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTypeInfoGetType
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfoType OcilibTypeInfoGetType
(
    OCI_TypeInfo *typinf
)
{
    GET_PROP
    (
        OCI_TypeInfoType, OCI_UNKNOWN,
        OCI_IPC_TYPE_INFO, typinf,
        type
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTypeInfoGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibTypeInfoGetConnection
(
    OCI_TypeInfo *typinf
)
{
    GET_PROP
    (
        OCI_Connection*, NULL,
        OCI_IPC_TYPE_INFO, typinf,
        con
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTypeInfoGetColumnCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibTypeInfoGetColumnCount
(
    OCI_TypeInfo *typinf
)
{
    GET_PROP
    (
        OCI_UInt32, 0,
        OCI_IPC_TYPE_INFO, typinf,
        nb_cols
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTypeInfoGetColumn
 * --------------------------------------------------------------------------------------------- */

OCI_Column * OcilibTypeInfoGetColumn
(
    OCI_TypeInfo *typinf,
    OCI_UInt32    index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Column*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);
    CHECK_INDEX(index, typinf->nb_cols);

    SET_RETVAL(&(typinf->cols[index]));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTypeInfoGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibTypeInfoGetName
(
    OCI_TypeInfo *typinf
)
{
    GET_PROP
    (
        const otext*, NULL,
        OCI_IPC_TYPE_INFO, typinf,
        name
    );
}

/* --------------------------------------------------------------------------------------------- *
* OcilibTypeInfoIsFinalType
* --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTypeInfoIsFinalType
(
    OCI_TypeInfo *typinf
)
{
    GET_PROP
    (
        OCI_Boolean, FALSE,
        OCI_IPC_TYPE_INFO, typinf,
        is_final
    );
}

/* --------------------------------------------------------------------------------------------- *
* OcilibTypeInfoGetSuperType
* --------------------------------------------------------------------------------------------- */

OCI_TypeInfo* OcilibTypeInfoGetSuperType
(
    OCI_TypeInfo *typinf
)
{
    GET_PROP
    (
        OCI_TypeInfo*, NULL,
        OCI_IPC_TYPE_INFO, typinf,
        parent_type
    );
}
