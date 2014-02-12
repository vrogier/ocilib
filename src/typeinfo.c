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
 * $Id: typeinfo.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_TypeInfoClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_TypeInfoClose
(
    OCI_TypeInfo *typinf
)
{
    ub2 i;

    OCI_CHECK(typinf == NULL, FALSE);

    for (i=0; i < typinf->nb_cols; i++)
    {
        OCI_FREE(typinf->cols[i].name);
    }

    OCI_FREE(typinf->cols);
    OCI_FREE(typinf->name);
    OCI_FREE(typinf->schema);
    OCI_FREE(typinf->offsets);

    return TRUE;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_TypeInfoGet
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OCI_API OCI_TypeInfoGet
(
    OCI_Connection *con,
    const otext    *name,
    unsigned int    type
)
{
    OCI_TypeInfo *typinf        = NULL;
    OCI_TypeInfo *syn_typinf    = NULL;
    OCI_Item *item              = NULL;
    OCIDescribe *dschp          = NULL;
    OCIParam *parmh1            = NULL;
    OCIParam *parmh2            = NULL;
    otext *str                  = NULL;
    int ptype                   = 0;
    ub1 desc_type               = 0;
    ub4 attr_type               = 0;
    ub4 num_type                = 0;
    boolean res                 = TRUE;
    boolean found               = FALSE;
    ub2 i;

    otext obj_schema[OCI_SIZE_OBJ_NAME+1];
    otext obj_name[OCI_SIZE_OBJ_NAME+1];

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, NULL);

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

    /* type name must be uppercase */

    for (str = obj_name; *str; str++)
    {
        *str = (otext) otoupper(*str);
    }

    /* schema name must be uppercase */

    for (str = obj_schema; *str; str++)
    {
        *str = (otext) otoupper(*str);
    }

    /* first try to find it in list */

    item = con->tinfs->head;

    /* walk along the list to find the type */

    while (item)
    {
        typinf = (OCI_TypeInfo *) item->data;

        if (typinf && (typinf->type == type))
        {
            if ((ostrcasecmp(typinf->name,   obj_name  ) == 0) &&
                (ostrcasecmp(typinf->schema, obj_schema) == 0))
            {
                found = TRUE;
                break;
            }
        }

        item = item->next;
    }

    /* Not found, so create type object */

    if (!found)
    {
        item = OCI_ListAppend(con->tinfs, sizeof(OCI_TypeInfo));

        res = (item != NULL);

        /* allocate describe handle */

        if (res)
        {
            typinf = (OCI_TypeInfo *) item->data;

            typinf->type        = type;
            typinf->con         = con;
            typinf->name        = ostrdup(obj_name);
            typinf->schema      = ostrdup(obj_schema);
            typinf->struct_size = 0;

            res = OCI_SUCCESSFUL(OCI_HandleAlloc(typinf->con->env,
                                                 (dvoid **) (void *) &dschp,
                                                 OCI_HTYPE_DESCRIBE, (size_t) 0,
                                                 (dvoid **) NULL));
        }

        /* perfom describe */

        if (res)
        {
            otext buffer[(OCI_SIZE_OBJ_NAME*2) + 2] = OTEXT("");

            size_t  size    = sizeof(buffer)/sizeof(otext);
            dbtext *dbstr1  = NULL;
            int     dbsize1 = -1;
            sb4     pbsp    = 1;

            str = buffer;

            /* compute full object name */

            if (typinf->schema && typinf->schema[0])
            {
                str   = ostrncat(buffer, typinf->schema, size);
                size -= ostrlen(typinf->schema);
                str   = ostrncat(str, OTEXT("."), size);
                size -= (size_t) 1;
            }

            ostrncat(str, typinf->name, size);

            dbstr1 = OCI_StringGetOracleString(str, &dbsize1);

            /* set public scope to include synonyms */
                
            OCI_CALL2
            (
                res, con,

                OCIAttrSet(dschp, OCI_HTYPE_DESCRIBE, &pbsp, (ub4) sizeof(pbsp), 
                            OCI_ATTR_DESC_PUBLIC, con->err)
            )

            /* describe call */

            OCI_CALL2
            (
                res, con,

                OCIDescribeAny(con->cxt, con->err, (dvoid *) dbstr1,
                                (ub4) dbsize1, OCI_OTYPE_NAME,
                                OCI_DEFAULT, OCI_PTYPE_UNK, dschp)
            )

            OCI_StringReleaseOracleString(dbstr1);

            /* get parameter handle */
                
            OCI_CALL2
            (
                res, con,

                OCIAttrGet(dschp, OCI_HTYPE_DESCRIBE, &parmh1,
                            NULL, OCI_ATTR_PARAM, con->err)
            )
            
            /* get object type */
                
            OCI_CALL2
            (
                res, con,

                OCIAttrGet(parmh1, OCI_DTYPE_PARAM, &desc_type,
                           NULL, OCI_ATTR_PTYPE, con->err)
            )
        }

        /* on sucessfull describe call, retrieve all information about the object 
           if it is not a synonym */

        if (res)
        {
            switch (desc_type)
            {
                case OCI_PTYPE_TYPE:
                {
                    if (OCI_UNKNOWN != typinf->type)
                    {
                        res = (OCI_TIF_TYPE == typinf->type);
                    }

                    typinf->type = OCI_TIF_TYPE;
                    
                    if (res)
                    {
                        boolean pdt     = FALSE;
                        dbtext *dbstr1  = NULL;
                        dbtext *dbstr2  = NULL;
                        int     dbsize1 = -1;
                        int     dbsize2 = -1;

                        attr_type = OCI_ATTR_LIST_TYPE_ATTRS;
                        num_type  = OCI_ATTR_NUM_TYPE_ATTRS;
                        ptype     = OCI_DESC_TYPE;

                        /* get the object  tdo */

                        dbstr1 = OCI_StringGetOracleString(typinf->schema, &dbsize1);
                        dbstr2 = OCI_StringGetOracleString(typinf->name,   &dbsize2);

                        OCI_CALL2
                        (
                            res, con,

                            OCITypeByName(typinf->con->env, con->err, con->cxt,
                                          (text *) dbstr1, (ub4) dbsize1,
                                          (text *) dbstr2, (ub4) dbsize2,
                                          (text *) NULL, (ub4) 0,
                                          OCI_DURATION_SESSION, OCI_TYPEGET_ALL,
                                          &typinf->tdo)
                        )

                        OCI_StringReleaseOracleString(dbstr1);
                        OCI_StringReleaseOracleString(dbstr2);

                        /* check if it's system predefined type if order to avoid the next call
                           that is not allowed on system types */

                        OCI_CALL2
                        (
                            res, con,

                            OCIAttrGet(parmh1, OCI_DTYPE_PARAM, &pdt,
                                       NULL, OCI_ATTR_IS_PREDEFINED_TYPE, con->err)
                        )

                        if (!pdt)
                        {
                            OCI_CALL2
                            (
                                res, con,

                                OCIAttrGet(parmh1, OCI_DTYPE_PARAM, &typinf->typecode,
                                           NULL, OCI_ATTR_TYPECODE, con->err)
                            )
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
                    if (OCI_UNKNOWN != typinf->type)
                    {
                        res = (((OCI_TIF_TABLE == typinf->type) && (OCI_PTYPE_VIEW != desc_type)) ||
                               ((OCI_TIF_VIEW  == typinf->type) && (OCI_PTYPE_VIEW == desc_type)));
                    }

                    typinf->type = (OCI_PTYPE_VIEW == desc_type ? OCI_TIF_VIEW : OCI_TIF_TABLE);
 
                    if (res)
                    {
                        attr_type = OCI_ATTR_LIST_COLUMNS;
                        num_type  = OCI_ATTR_NUM_COLS;
                        ptype     = OCI_DESC_TABLE;             
                    }
                    
                    break;
                }
                case OCI_PTYPE_SYN:
                {
                    otext *syn_schema_name   = NULL;
                    otext *syn_object_name   = NULL;
                    otext *syn_link_name     = NULL;

                    otext syn_fullname[(OCI_SIZE_OBJ_NAME*3) + 3] = OTEXT("");

                    /* get link schema, object and databaselink names */

                    res = res && OCI_StringGetFromAttrHandle (con, parmh1, OCI_DTYPE_PARAM,
                                                              OCI_ATTR_SCHEMA_NAME, 
                                                              &syn_schema_name);
                    
                    res = res && OCI_StringGetFromAttrHandle (con, parmh1, OCI_DTYPE_PARAM,
                                                              OCI_ATTR_NAME, 
                                                              &syn_object_name);
 
                    res = res && OCI_StringGetFromAttrHandle (con, parmh1, OCI_DTYPE_PARAM,
                                                              OCI_ATTR_LINK, &syn_link_name);

                    /* compute link full name */
                    
                    if (syn_schema_name && syn_schema_name[0])
                    {
                        ostrncat(syn_fullname, syn_schema_name, (size_t) OCI_SIZE_OBJ_NAME);
                        ostrncat(syn_fullname, OTEXT("."), 1);
                    }

                    if (syn_object_name && syn_object_name[0])
                    {
                        ostrncat(syn_fullname, syn_object_name, (size_t) OCI_SIZE_OBJ_NAME);
                    }
                
                    if (syn_link_name && syn_link_name[0])
                    {
                        ostrncat(syn_fullname, OTEXT("@"), 1);
                        ostrncat(syn_fullname, syn_link_name, (size_t) OCI_SIZE_OBJ_NAME);
                    }

                    /* retrieve the type info of the real object */

                    syn_typinf = OCI_TypeInfoGet (con, syn_fullname, typinf->type);
                         
                    /* free temporay strings */

                    OCI_MemFree (syn_link_name);
                    OCI_MemFree (syn_object_name);
                    OCI_MemFree (syn_schema_name);
                    
                    /* do we have a valid object ? */

                    res = (syn_typinf != NULL);

                    break;
                }
            }

            /*  did we handle a supported object other than a synonym */

            if (res && (OCI_UNKNOWN != ptype)) 
            {
                /* do we need get more attributes for collections ? */

                if (SQLT_NCO == typinf->typecode)
                {
                    typinf->nb_cols = 1;

                    ptype  = OCI_DESC_COLLECTION;
                    parmh2 = parmh1;

                    OCI_CALL2
                    (
                        res, con,

                        OCIAttrGet(parmh1, OCI_DTYPE_PARAM, &typinf->colcode,
                                   NULL, OCI_ATTR_COLLECTION_TYPECODE, con->err)
                    )
                }
                else
                {
                    OCI_CALL2
                    (
                        res, con,

                        OCIAttrGet(parmh1, OCI_DTYPE_PARAM, &parmh2,
                                   NULL, attr_type, con->err)
                    )

                    OCI_CALL2
                    (
                        res, con,

                        OCIAttrGet(parmh1, OCI_DTYPE_PARAM, &typinf->nb_cols,
                                   NULL, num_type, con->err)
                    )
                }

                /* allocates memory for cached offsets */

                if (typinf->nb_cols > 0)
                {
                    typinf->offsets = (int *) OCI_MemAlloc(OCI_IPC_ARRAY,
                                                           sizeof(*typinf->offsets),
                                                           (size_t) typinf->nb_cols,
                                                           FALSE);

                    res = (typinf->offsets != NULL);

                    if (res)
                    {
                        memset(typinf->offsets, -1, sizeof(*typinf->offsets) * typinf->nb_cols);
                    }
                }

                /* allocates memory for children */

                if (typinf->nb_cols > 0)
                {
                    typinf->cols = (OCI_Column *) OCI_MemAlloc(OCI_IPC_COLUMN,  sizeof(*typinf->cols),
                                                               (size_t) typinf->nb_cols, TRUE);

                    /* describe children */

                    if (typinf->cols)
                    {
                        for (i = 0; i < typinf->nb_cols; i++)
                        {
                            res = res && OCI_ColumnDescribe(&typinf->cols[i], con,
                                                            NULL, parmh2, i + 1, ptype);

                            res = res && OCI_ColumnMap(&typinf->cols[i], NULL);

                            if (res == FALSE)
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        res = FALSE;
                    }
                }
            }
        }
    }

    /* free describe handle */

    if (dschp)
    {
        OCI_HandleFree(dschp, OCI_HTYPE_DESCRIBE);
    }

    /* increment type info reference counter on success */

    if (typinf)
    {
        typinf->refcount++;

        /* type checking sanity checks */

        if ((type != OCI_UNKNOWN) && (typinf->type != type))
        {
            OCI_ExceptionTypeInfoWrongType(con, name);

            res = FALSE;
        }
    }

    /* handle errors */

    if (!res || syn_typinf)
    {
        OCI_TypeInfoFree(typinf);
        typinf = NULL;
    }

    OCI_RESULT(res);

    return syn_typinf ? syn_typinf : typinf;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TypeInfoFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TypeInfoFree
(
    OCI_TypeInfo *typinf
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, FALSE);

    typinf->refcount--;

    if (typinf->refcount == 0)
    {
        OCI_ListRemove(typinf->con->tinfs, typinf);

        res = OCI_TypeInfoClose(typinf);

        OCI_FREE(typinf);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TypeInfoGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_TypeInfoGetType
(
    OCI_TypeInfo *typinf
)
{
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return typinf->type;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TypeInfoGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_TypeInfoGetConnection
(
    OCI_TypeInfo *typinf
)
{
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);

    OCI_RESULT(TRUE);

    return typinf->con;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TypeInfoGetColumnCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_TypeInfoGetColumnCount
(
    OCI_TypeInfo *typinf
)
{
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, 0);

    OCI_RESULT(TRUE);

    return typinf->nb_cols;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TypeInfoGetColumn
 * --------------------------------------------------------------------------------------------- */

OCI_Column * OCI_API OCI_TypeInfoGetColumn
(
    OCI_TypeInfo *typinf,
    unsigned int  index
)
{
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);
    OCI_CHECK_BOUND(typinf->con, index, 1,  typinf->nb_cols, NULL);

    OCI_RESULT(TRUE);

    return &(typinf->cols[index-1]);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TypeInfoGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_TypeInfoGetName
(
    OCI_TypeInfo *typinf
)
{
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);

    OCI_RESULT(TRUE);

    return typinf->name;
}
