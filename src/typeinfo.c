/*
   +----------------------------------------------------------------------+   
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2009 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+ 
*/

/* ------------------------------------------------------------------------ *
 * $Id: typeinfo.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_TypeInfoClose
 * ------------------------------------------------------------------------ */

boolean OCI_TypeInfoClose(OCI_TypeInfo *typinf)
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

    return TRUE;
}

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_TypeInfoGet
 * ------------------------------------------------------------------------ */

OCI_TypeInfo * OCI_API OCI_TypeInfoGet(OCI_Connection *con, const mtext *name, 
                                       unsigned int type)
{
    OCI_TypeInfo *typinf = NULL;
    OCI_Item     *item   = NULL;
    OCIDescribe *dschp   = NULL;
    OCIParam *parmh1     = NULL;
    OCIParam *parmh2     = NULL;
    mtext *str           = NULL;
    int etype            = OCI_DESC_COLUMN;
    int ptype            = 0;
    ub1 item_type        = 0;
    ub4 attr_type        = 0;
    ub4 num_type         = 0;
    boolean res          = TRUE;
    boolean found        = FALSE;
    ub2 i;
    
    mtext obj_schema[OCI_SIZE_OBJ_NAME+1];
    mtext obj_name[OCI_SIZE_OBJ_NAME+1];

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, NULL);

    if (type == OCI_TIF_TABLE)
        item_type = OCI_PTYPE_TABLE;
    else if (type == OCI_TIF_VIEW)
        item_type = OCI_PTYPE_VIEW;
    else if (type == OCI_TIF_TYPE)
        item_type = OCI_PTYPE_TYPE;
    else
        return NULL;

    obj_schema[0] = 0;
    obj_name[0] = 0;

    /* is the schema provided in the object name ? */

    for (str = (mtext *) name; *str != 0; str++)
    {
        if (*str == MT('.'))
        {
            mtsncat(obj_schema, name, str-name);
            mtsncat(obj_name, ++str, OCI_SIZE_OBJ_NAME);
            break;
        }
    }

    /* if the schema is not provided, we just copy the object name */

    if (obj_name[0] == 0)
    {
        mtsncat(obj_name, name, OCI_SIZE_OBJ_NAME);
    }

    /* type name must be uppercase */

    for (str = obj_name; *str != 0; str++)
        *str = (mtext) mttoupper(*str);

    /* schema name must be uppercase */

    for (str = obj_schema; *str != 0; str++)
        *str = (mtext) mttoupper(*str);

    /* first try to find it in list */

    item = con->tinfs->head;

    /* walk along the list to find the type */

    while (item != NULL)
    {
        typinf = (OCI_TypeInfo *) item->data;
     
        if ((typinf != NULL) && (typinf->type == type))
        {
            if ((mtscasecmp(typinf->name,   obj_name  ) == 0) &&
                (mtscasecmp(typinf->schema, obj_schema) == 0))
            {
                found = TRUE;
                break;
            }
        }

        item = item->next;
    } 

    /* Not found, so create type object */

    if (found == FALSE)
    {
        item = OCI_ListAppend(con->tinfs, sizeof(OCI_TypeInfo));

        res = (item != NULL);

        if (res == TRUE)
        {
            typinf = (OCI_TypeInfo *) item->data;

            typinf->type   = type;
            typinf->con    = con;
            typinf->name   = mtsdup(obj_name);
            typinf->schema = mtsdup(obj_schema);

            res = (OCI_SUCCESS == OCI_HandleAlloc(OCILib.env,
                                                  (dvoid **) (void *) &dschp, 
                                                  OCI_HTYPE_DESCRIBE, 0, 
                                                  (dvoid **) NULL));
        }

        if (res == TRUE)
        {
            if (type == OCI_TIF_TYPE)
            {            
                void *ostr1 = NULL;
                void *ostr2 = NULL;
                int osize1  = -1;
                int osize2  = -1;
               
                attr_type = OCI_ATTR_LIST_TYPE_ATTRS;
                num_type  = OCI_ATTR_NUM_TYPE_ATTRS;
                ptype     = OCI_DESC_TYPE;

                ostr1 = OCI_GetInputMetaString(typinf->schema, &osize1);
                ostr2 = OCI_GetInputMetaString(typinf->name,   &osize2);

                OCI_CALL2
                (
                    res, con, 
                    
                    OCITypeByName(OCILib.env, con->err, con->cxt,
                                 (text *) ostr1, (ub4) osize1, 
                                 (text *) ostr2, (ub4) osize2, 
                                 (text *) NULL, (ub4) 0, 
                                 OCI_DURATION_SESSION, OCI_TYPEGET_ALL,
                                 &typinf->tdo)
                )

                OCI_CALL2
                (
                    res, con, 
                    
                    OCIDescribeAny(con->cxt, con->err, (void *) typinf->tdo,
                                   0, OCI_OTYPE_PTR, OCI_DEFAULT, 
                                   OCI_PTYPE_TYPE, dschp)
                )
                
                OCI_ReleaseMetaString(ostr1);
                OCI_ReleaseMetaString(ostr2);
            }
            else
            {
                mtext buffer[(OCI_SIZE_OBJ_NAME*2) + 2];
                
                size_t size = sizeof(buffer)/sizeof(mtext);
                void *ostr1 = NULL;
                int osize1  = -1;

                attr_type = OCI_ATTR_LIST_COLUMNS;
                num_type  = OCI_ATTR_NUM_COLS;
                ptype     = OCI_DESC_TABLE;
                str       = buffer;

                str[0] = 0;                

                if ((typinf->schema != NULL) && (typinf->schema[0] != 0))
                {
                    str = mtsncat(buffer, typinf->schema, size);
                    size -= mtslen(typinf->schema);
                    str = mtsncat(str, MT("."), size);
                    size -= 1;
                }

                mtsncat(str, typinf->name, size);

                ostr1 = OCI_GetInputMetaString(str, &osize1);

                OCI_CALL2
                (
                    res, con, 
                    
                    OCIDescribeAny(con->cxt, con->err, (dvoid *) ostr1, 
                                   (ub4) osize1, OCI_OTYPE_NAME, 
                                   OCI_DEFAULT, item_type, dschp)
                )

                OCI_ReleaseMetaString(ostr1);
            }
                  
            OCI_CALL2
            (
                res, con, 
                
                OCIAttrGet(dschp, OCI_HTYPE_DESCRIBE, &parmh1, 
                           NULL, OCI_ATTR_PARAM, con->err)
            )

            /* do we need get more attributes for collections ? */

            if (type == OCI_TIF_TYPE)
            {
                OCI_CALL2
                (
                    res, con, 
                    
                    OCIAttrGet(parmh1, OCI_DTYPE_PARAM, &typinf->tcode, 
                               NULL, OCI_ATTR_TYPECODE, con->err)
                )

            }

            if (typinf->tcode == SQLT_NCO)
            {      
                typinf->nb_cols = 1;
                
                ptype  = OCI_DESC_COLLECTION;
                etype  = OCI_DESC_TYPE;
                parmh2 = parmh1;

                OCI_CALL2
                (
                    res, con, 
                    
                    OCIAttrGet(parmh1, OCI_DTYPE_PARAM, &typinf->ccode, 
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

            /* allocates memory for children */

            if (typinf->nb_cols > 0)
            {
                typinf->cols = (OCI_Column *) OCI_MemAlloc(OCI_IPC_COLUMN,
                                                           sizeof(*typinf->cols),
                                                           typinf->nb_cols,
                                                           TRUE);

                /* describe children */

                if (typinf->cols != NULL)
                {
                    for (i = 0; i < typinf->nb_cols; i++)
                    {
                        res = res && OCI_ColumnDescribe(&typinf->cols[i], con, 
                                                        NULL, parmh2, i + 1, ptype);
                        
                        res = res && OCI_ColumnMap(&typinf->cols[i], NULL);

                        if (res == FALSE)
                            break;
                   }
                }
                else
                    res = FALSE;
            }
  
            /* free describe handle */

            if (dschp != NULL)
                OCI_HandleFree(dschp, OCI_HTYPE_DESCRIBE);
        }
    }

   /* handle errors */

    if (res == FALSE)
    {
       OCI_TypeInfoFree(typinf);        
       typinf = NULL;
    }

    OCI_RESULT(res);

    /* increment type info reference counter on success */

    if (typinf != NULL)
        typinf->refcount++;

    return typinf; 
}

/* ------------------------------------------------------------------------ *
 * OCI_TypeInfoFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TypeInfoFree(OCI_TypeInfo *typinf)
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

/* ------------------------------------------------------------------------ *
 * OCI_TypeInfoGetType
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_TypeInfoGetType(OCI_TypeInfo *typinf)
{
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return typinf->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_TypeInfoGetColumnCount
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_TypeInfoGetColumnCount(OCI_TypeInfo *typinf)
{
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, 0);
  
    OCI_RESULT(TRUE);

    return typinf->nb_cols;
}

/* ------------------------------------------------------------------------ *
 * OCI_TypeInfoGetColumn
 * ------------------------------------------------------------------------ */

OCI_Column * OCI_API OCI_TypeInfoGetColumn(OCI_TypeInfo *typinf, unsigned int index)
{
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);
    OCI_CHECK_BOUND(typinf->con, index, 1,  typinf->nb_cols, NULL);

    OCI_RESULT(TRUE);

    return &(typinf->cols[index-1]);
}

/* ------------------------------------------------------------------------ *
 * OCI_TypeInfoGetName
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_TypeInfoGetName(OCI_TypeInfo *typinf)
{
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);

    OCI_RESULT(TRUE);

    return typinf->name;
}
