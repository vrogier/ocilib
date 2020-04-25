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

#include "typeinfo.h"

#include "column.h"
#include "list.h"
#include "macros.h"
#include "memory.h"
#include "strings.h"

static unsigned int TypeInfoTypeValues[] = { OCI_TIF_TABLE, OCI_TIF_VIEW, OCI_TIF_TYPE };

typedef struct TypeInfoFindParams
{
    unsigned int type;
    otext      * schema;
    otext      * name;
} TypeInfoFindParams;

/* --------------------------------------------------------------------------------------------- *
 * TypeInfoFind
 * --------------------------------------------------------------------------------------------- */

boolean TypeInfoFind(OCI_TypeInfo *typinf, TypeInfoFindParams *find_params)
{
    return
        typinf &&
        find_params &&
        typinf->type == find_params->type &&
        ostrcasecmp(typinf->name, find_params->name) == 0 &&
        ostrcasecmp(typinf->schema, find_params->schema) == 0;
}

/* --------------------------------------------------------------------------------------------- *
 * TypeInfoDispose
 * --------------------------------------------------------------------------------------------- */

boolean TypeInfoDispose
(
    OCI_TypeInfo *typinf
)
{
    CHECK(NULL == typinf, FALSE);

    for (ub2 i = 0; i < typinf->nb_cols; i++)
    {
        FREE(typinf->cols[i].name)
    }

    FREE(typinf->cols)
    FREE(typinf->name)
    FREE(typinf->schema)
    FREE(typinf->offsets)

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * TypeInfoGet
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * TypeInfoGet
(
    OCI_Connection *con,
    const otext    *name,
    unsigned int    type
)
{
    TypeInfoFindParams find_params;

    OCI_TypeInfo *typinf     = NULL;
    OCI_TypeInfo *syn_typinf = NULL;
    OCIDescribe  *dschp      = NULL;
    OCIParam     *param_root = NULL;
    OCIParam     *param_type = NULL;
    OCIParam     *param_cols = NULL;
    OCIParam     *param_list = NULL;
    otext        *str        = NULL;
    int           ptype      = 0;
    ub1           desc_type  = 0;
    ub4           attr_type  = 0;
    ub4           num_type   = 0;

    otext obj_schema[OCI_SIZE_OBJ_NAME + 1];
    otext obj_name[OCI_SIZE_OBJ_NAME + 1];

    CALL_ENTER(OCI_TypeInfo*, NULL)
    CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    CALL_CHECK_PTR(OCI_IPC_STRING, name)
    CALL_CHECK_ENUM_VALUE(con, NULL, type, TypeInfoTypeValues, OTEXT("Type"))
    CALL_CONTEXT_FROM_CON(con)

    obj_schema[0] = 0;
    obj_name[0]   = 0;

    /* is the schema provided in the object name ? */

    for (str = (otext *) name; *str; str++)
    {
        if (*str == OTEXT('.'))
        {
            ostrncat(obj_schema, name, str-name);
            ostrncat(obj_name, ++str, (size_t) OCI_SIZE_OBJ_NAME);
            break;
        }
    }

    /* if the schema is not provided, we just copy the object name */

    if (!obj_name[0])
    {
        ostrncat(obj_name, name, (size_t) OCI_SIZE_OBJ_NAME);
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

    typinf = ListFind(con->tinfs, (POCI_LIST_FIND) TypeInfoFind, &find_params);

    /* Not found, so create type object */

    if (!typinf)
    {
        typinf = ListAppend(con->tinfs, sizeof(OCI_TypeInfo));
        STATUS = (NULL != typinf);

        /* allocate describe handle */

        if (STATUS)
        {
            typinf->con         = con;
            typinf->name        = ostrdup(obj_name);
            typinf->schema      = ostrdup(obj_schema);
            typinf->struct_size = 0;
            typinf->align       = 0;

            STATUS = MemoryAllocHandle(typinf->con->env, (dvoid **)(void *)&dschp, OCI_HTYPE_DESCRIBE);
        }

        /* perform describe */

        if (STATUS)
        {
            otext buffer[(OCI_SIZE_OBJ_NAME * 2) + 2] = OTEXT("");

            size_t  max_chars = sizeof(buffer) / sizeof(otext) - 1;
            dbtext *dbstr1    = NULL;
            int     dbsize1   = -1;
            sb4     pbsp      = 1;

            str = buffer;

            /* compute full object name */

            if (IS_STRING_VALID(typinf->schema))
            {
                str        = ostrncat(buffer, typinf->schema, max_chars);
                max_chars -= ostrlen(typinf->schema);

                str        = ostrncat(str, OTEXT("."), max_chars);
                max_chars -= (size_t)1;
            }

            ostrncat(str, typinf->name, max_chars);

            dbstr1 = StringGetDBString(str, &dbsize1);

            /* set public scope to include synonyms */

            ATTRIB_SET(OCI_HTYPE_DESCRIBE, OCI_ATTR_DESC_PUBLIC, dschp, &pbsp, sizeof(pbsp))

            /* describe call */

            EXEC
            (
                OCIDescribeAny(con->cxt, con->err, (dvoid *) dbstr1,
                               (ub4) dbsize1, OCI_OTYPE_NAME,
                               OCI_DEFAULT, OCI_PTYPE_UNK, dschp)
            )

            StringReleaseDBString(dbstr1);

            /* get parameter handle */

            ATTRIB_GET(OCI_HTYPE_DESCRIBE, OCI_ATTR_PARAM, dschp, &param_root, NULL)

            /* get describe type */

            ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_PTYPE, param_root, &desc_type, NULL)
        }

        /* on successful describe call, retrieve all information about the object
           if it is not a synonym */

        if (STATUS)
        {
            switch (desc_type)
            {
                case OCI_PTYPE_TYPE:
                case OCI_PTYPE_LIST:
                {
                    ub1     pdt = 0; /* Is Predefined Type */
                    ub1     ist = 0; /* Is Sub Type */
                    OCIRef *ref = NULL;

                    STATUS = (OCI_TIF_TYPE == type);

                    if (STATUS)
                    {
                        typinf->type = OCI_TIF_TYPE;

                        if (OCI_PTYPE_LIST == desc_type)
                        {
                            EXEC(OCIParamGet((dvoid *)param_root, OCI_DTYPE_PARAM, con->err, (void**)&param_type, (ub4)0))
                        }
                        else
                        {
                            param_type = param_root;
                        }

                        /* get the object type descriptor */

                        ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_REF_TDO, param_type, &ref, NULL)

                        EXEC(OCITypeByRef(typinf->con->env, con->err, ref, OCI_DURATION_SESSION, OCI_TYPEGET_ALL, &typinf->tdo))

                        /* check if the type is not final, e.g. can be inherited */

                        ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_IS_FINAL_TYPE, param_type, &typinf->is_final, NULL)

                        /* check if it's system predefined type if order to avoid the next call that
                         * is not allowed on system types */

                        ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_IS_PREDEFINED_TYPE, param_type, &pdt, NULL)

                        if (!pdt)
                        {
                            ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_TYPECODE, param_type, &typinf->typecode, NULL)
                        }

                        /* check if the type is a subtype derived from a super type */

                        ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_IS_SUBTYPE, param_type, &ist, NULL)

                        if (ist)
                        {
                            otext *sp_schema_name = NULL;
                            otext *sp_object_name = NULL;

                            unsigned int size_schema = 0;
                            unsigned int size_object = 0;

                            otext sp_fullname[(OCI_SIZE_OBJ_NAME * 2) + 2] = OTEXT("");

                            /* get super type schema and name */

                            STATUS = STATUS && StringGetAttribute(con, param_root, OCI_DTYPE_PARAM,
                                                                  OCI_ATTR_SUPERTYPE_SCHEMA_NAME,
                                                                  &sp_schema_name, &size_schema);

                            STATUS = STATUS && StringGetAttribute(con, param_root, OCI_DTYPE_PARAM,
                                                                  OCI_ATTR_SUPERTYPE_NAME,
                                                                  &sp_object_name, &size_object);

                            /* compute super type full name */

                            StringGetFullTypeName(sp_schema_name, NULL, sp_object_name, NULL, sp_fullname,
                                                  (sizeof(sp_fullname) / sizeof(otext)) - 1);

                            /* retrieve the type info of the real object */

                            typinf->parent_type = TypeInfoGet(con, sp_fullname, type);

                            /* free temporary strings */

                            MemoryFree (sp_schema_name);
                            MemoryFree (sp_object_name);

                            STATUS = (NULL != typinf->parent_type);
                        }

                        switch (typinf->typecode)
                        {
                            case  SQLT_NTY:
#if OCI_VERSION_COMPILE >= OCI_12_1
                            case  SQLT_REC:
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

                                ATTRIB_GET(OCI_DTYPE_PARAM, OCI_ATTR_COLLECTION_TYPECODE, param_type, &typinf->colcode, NULL)
                                break;
                            }
                            default:
                            {
                                STATUS = FALSE;
                                ExceptionDatatypeNotSupported(con, NULL, typinf->typecode);
                                break;
                            }
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
                    STATUS = (((OCI_TIF_TABLE == type) && (OCI_PTYPE_VIEW != desc_type)) ||
                              ((OCI_TIF_VIEW  == type) && (OCI_PTYPE_VIEW == desc_type)));

                    if (STATUS)
                    {
                        typinf->type = (OCI_PTYPE_VIEW == desc_type ? OCI_TIF_VIEW : OCI_TIF_TABLE);
                        attr_type    = OCI_ATTR_LIST_COLUMNS;
                        num_type     = OCI_ATTR_NUM_COLS;
                        ptype        = OCI_DESC_TABLE;
                        param_list   = param_root;
                    }

                    break;
                }
                case OCI_PTYPE_SYN:
                {
                    otext *syn_schema_name = NULL;
                    otext *syn_object_name = NULL;
                    otext *syn_link_name   = NULL;

                    unsigned int size_schema = 0;
                    unsigned int size_object = 0;
                    unsigned int size_link   = 0;

                    otext syn_fullname[(OCI_SIZE_OBJ_NAME * 3) + 3] = OTEXT("");

                    /* get link schema, object and database link names */

                    STATUS = STATUS && StringGetAttribute(con, param_root, OCI_DTYPE_PARAM,
                                                          OCI_ATTR_SCHEMA_NAME, &syn_schema_name, &size_schema);

                    STATUS = STATUS && StringGetAttribute(con, param_root, OCI_DTYPE_PARAM,
                                                          OCI_ATTR_NAME, &syn_object_name, &size_object);

                    STATUS = STATUS && StringGetAttribute(con, param_root, OCI_DTYPE_PARAM,
                                                          OCI_ATTR_LINK, &syn_link_name, &size_link);

                    /* compute link full name */

                    StringGetFullTypeName(syn_schema_name, NULL, syn_object_name, syn_link_name, syn_fullname,
                                          (sizeof(syn_fullname) / sizeof(otext)) - 1);

                    /* retrieve the type info of the real object */

                    syn_typinf = TypeInfoGet (con, syn_fullname, type);

                    /* free temporary strings */

                    MemoryFree (syn_link_name);
                    MemoryFree(syn_object_name);
                    MemoryFree(syn_schema_name);

                    /* do we have a valid object ? */

                    STATUS = (NULL != syn_typinf);

                    break;
                }
            }

            /*  did we handle a supported object other than a synonym */

            if (STATUS && (OCI_UNKNOWN != ptype))
            {
                /* retrieve the columns parameter if not already retrieved */
                if (param_list)
                {
                    ATTRIB_GET(OCI_DTYPE_PARAM, attr_type, param_list, &param_cols, NULL)
                    ATTRIB_GET(OCI_DTYPE_PARAM, num_type, param_list, &typinf->nb_cols, NULL)
                }

                /* allocates memory for cached offsets */

                if (typinf->nb_cols > 0)
                {
                    ALLOC_DATA(OCI_IPC_ARRAY, typinf->offsets, typinf->nb_cols)

                    if (STATUS)
                    {
                        memset(typinf->offsets, -1, sizeof(*typinf->offsets) * typinf->nb_cols);
                    }
                }

                /* allocates memory for children */

                if (typinf->nb_cols > 0)
                {
                    ALLOC_DATA(OCI_IPC_COLUMN, typinf->cols, typinf->nb_cols)

                    /* describe children */
                    if (STATUS)
                    {
                        for (ub2 i = 0; i < typinf->nb_cols; i++)
                        {
                            STATUS = STATUS && ColumnRetrieveInfo(&typinf->cols[i], con, NULL, param_cols, i + 1, ptype);
                            STATUS = STATUS && ColumnMapInfo(&typinf->cols[i], NULL);

                            if (!STATUS)
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        STATUS = FALSE;
                    }
                }
            }
        }
    }

    /* free describe handle */

    MemoryFreeHandle(dschp, OCI_HTYPE_DESCRIBE);

    /* increment type info reference counter on success */

    if (typinf && STATUS)
    {
        typinf->refcount++;

        /* type checking sanity checks */

        if ((type != OCI_UNKNOWN) && ((syn_typinf && syn_typinf->type != type) || (!syn_typinf && typinf->type != type)))
        {
            ExceptionTypeInfoWrongType(con, name);

            STATUS = FALSE;
        }
    }

    /* handle errors */

    if (!STATUS || syn_typinf)
    {
        TypeInfoFree(typinf);
    }

    if (STATUS)
    {
        RETVAL = syn_typinf ? syn_typinf : typinf;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TypeInfoFree
 * --------------------------------------------------------------------------------------------- */

boolean TypeInfoFree
(
    OCI_TypeInfo *typinf
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CALL_CONTEXT_FROM_CON(typinf->con)

    typinf->refcount--;

    if (typinf->refcount == 0)
    {
        ListRemove(typinf->con->tinfs, typinf);

        TypeInfoDispose(typinf);

        FREE(typinf)
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TypeInfoGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int TypeInfoGetType
(
    OCI_TypeInfo *typinf
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_TYPE_INFO, typinf, type, typinf->con, NULL, typinf->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * TypeInfoGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * TypeInfoGetConnection
(
    OCI_TypeInfo *typinf
)
{
    GET_PROP(OCI_Connection*, NULL, OCI_IPC_TYPE_INFO, typinf, con, typinf->con, NULL, typinf->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * TypeInfoGetColumnCount
 * --------------------------------------------------------------------------------------------- */

unsigned int TypeInfoGetColumnCount
(
    OCI_TypeInfo *typinf
)
{
    GET_PROP(unsigned int, 0, OCI_IPC_TYPE_INFO, typinf, nb_cols, typinf->con, NULL, typinf->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * TypeInfoGetColumn
 * --------------------------------------------------------------------------------------------- */

OCI_Column * TypeInfoGetColumn
(
    OCI_TypeInfo *typinf,
    unsigned int  index
)
{
    CALL_ENTER(OCI_Column *, NULL)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CALL_CHECK_BOUND(typinf->con, index, 1,  typinf->nb_cols)
    CALL_CONTEXT_FROM_CON(typinf->con)

    RETVAL = &(typinf->cols[index - 1]);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TypeInfoGetName
 * --------------------------------------------------------------------------------------------- */

const otext * TypeInfoGetName
(
    OCI_TypeInfo *typinf
)
{
    GET_PROP(const otext*, NULL, OCI_IPC_TYPE_INFO, typinf, name, typinf->con, NULL, typinf->con->err)
}

/* --------------------------------------------------------------------------------------------- *
* OCI_TypeInfoIsFinalType
* --------------------------------------------------------------------------------------------- */

boolean TypeInfoIsFinalType
(
    OCI_TypeInfo *typinf
)
{
    GET_PROP(boolean, TRUE, OCI_IPC_TYPE_INFO, typinf, is_final, typinf->con, NULL, typinf->con->err)
}

/* --------------------------------------------------------------------------------------------- *
* OCI_TypeInfoGetSuperType
* --------------------------------------------------------------------------------------------- */

OCI_TypeInfo* TypeInfoGetSuperType
(
    OCI_TypeInfo *typinf
)
{
    GET_PROP(OCI_TypeInfo*, NULL, OCI_IPC_TYPE_INFO, typinf, parent_type, typinf->con, NULL, typinf->con->err)
}
