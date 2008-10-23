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
   |               Copyright (c) 2007-2008 Vincent ROGIER                 |
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
 * $Id: schema.c, v 3.1.0 2008/10/23 21:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_SchemaClose
 * ------------------------------------------------------------------------ */

boolean OCI_SchemaClose(OCI_Schema *schema)
{
    ub2 i;

    OCI_CHECK(schema == NULL, FALSE);

    for (i=0; i < schema->nb_cols; i++)
    {
        OCI_FREE(schema->cols[i].name);
    }

    OCI_FREE(schema->cols);
    OCI_FREE(schema->name);

    return TRUE;
}

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_SchemaGet
 * ------------------------------------------------------------------------ */

OCI_Schema * OCI_API OCI_SchemaGet(OCI_Connection *con, const mtext *name, 
                                   unsigned int type)
{
    OCI_Schema *schema = NULL;
    OCI_Item     *item = NULL;
    OCIDescribe *dschp = NULL;
    OCIParam *parmh1   = NULL;
    OCIParam *parmh2   = NULL;
    void *ostr         = NULL;
    mtext *str         = NULL;
    int etype          = OCI_DESC_COLUMN;
    int ptype          = 0;
    ub1 item_type      = 0;
    ub4 attr_type      = 0;
    ub4 num_type       = 0;
    int osize          = -1;
    boolean res        = TRUE;
    boolean found      = FALSE;
    ub2 i;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);
    OCI_CHECK_PTR(OCI_IPC_STRING, name, NULL);

    if (type == OCI_SCHEMA_TABLE)
        item_type = OCI_PTYPE_TABLE;
    else if (type == OCI_SCHEMA_VIEW)
        item_type = OCI_PTYPE_VIEW;
    else if (type == OCI_SCHEMA_TYPE)
        item_type = OCI_PTYPE_TYPE;
    else
        return NULL;

    /* first try to find it in list */

    item = con->sobjs->head;

    /* walk along the list to find the type */
    while (item != NULL)
    {
        schema = (OCI_Schema *) item->data;
     
        if (schema != NULL && mtscasecmp(schema->name, name) == 0)
        {
            if (schema->type == type)
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
        item = OCI_ListAppend(con->sobjs, sizeof(OCI_Schema));

        res = (item != NULL);

        if (res == TRUE)
        {
            schema = (OCI_Schema *) item->data;

            schema->type = type;
            schema->con  = con;
            schema->name = mtsdup(name);

            /* type name must be uppercase */

            for (str = schema->name; *str != 0; str++)
                *str = (mtext) mttoupper((short)*str);

            res = (OCI_SUCCESS == OCI_HandleAlloc(OCILib.env,
                                                  (dvoid **) (void *) &dschp, 
                                                  OCI_HTYPE_DESCRIBE, 0, 
                                                  (dvoid **) NULL));
        }

        if (res == TRUE)
        {
            ostr = OCI_GetInputMetaString(schema->name, &osize);

            if (type == OCI_SCHEMA_TYPE)
            {            
                attr_type = OCI_ATTR_LIST_TYPE_ATTRS;
                num_type  = OCI_ATTR_NUM_TYPE_ATTRS;
                ptype     = OCI_DESC_TYPE;

                OCI_CALL2
                (
                    res, con, 
                    
                    OCITypeByName(OCILib.env, con->err, con->cxt,
                                 (text *) NULL, (ub4) 0, 
                                 (text *) ostr, (ub4) osize, 
                                 (text *) NULL, (ub4) 0, 
                                 OCI_DURATION_SESSION, OCI_TYPEGET_ALL,
                                 &schema->tdo)
                )

                OCI_CALL2
                (
                    res, con, 
                    
                    OCIDescribeAny(con->cxt, con->err, (void *) schema->tdo,
                                   0, OCI_OTYPE_PTR, OCI_DEFAULT, 
                                   OCI_PTYPE_TYPE, dschp)
                )
            }
            else
            {
                attr_type = OCI_ATTR_LIST_COLUMNS;
                num_type  = OCI_ATTR_NUM_COLS;
                ptype     = OCI_DESC_TABLE;

                OCI_CALL2
                (
                    res, con, 
                    
                    OCIDescribeAny(con->cxt, con->err, (dvoid *) ostr, 
                                   (ub4) osize, OCI_OTYPE_NAME, 
                                   OCI_DEFAULT, item_type, dschp)
                 )
            }
                  
            OCI_ReleaseMetaString(ostr);

            OCI_CALL2
            (
                res, con, 
                
                OCIAttrGet(dschp, OCI_HTYPE_DESCRIBE, &parmh1, 
                           NULL, OCI_ATTR_PARAM, con->err)
            )

            /* do we need get more attributes for Collections ? */

            if (type == OCI_SCHEMA_TYPE)
            {
                OCI_CALL2
                (
                    res, con, 
                    
                    OCIAttrGet(parmh1, OCI_DTYPE_PARAM, &schema->tcode, 
                               NULL, OCI_ATTR_TYPECODE, con->err)
                )

            }

            if (schema->tcode == SQLT_NCO)
            {      
                schema->nb_cols = 1;
                
                ptype  = OCI_DESC_COLLECTION;
                etype  = OCI_DESC_TYPE;
                parmh2 = parmh1;

                OCI_CALL2
                (
                    res, con, 
                    
                    OCIAttrGet(parmh1, OCI_DTYPE_PARAM, &schema->ccode, 
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
                    
                    OCIAttrGet(parmh1, OCI_DTYPE_PARAM, &schema->nb_cols,
                               NULL, num_type, con->err)
                )
            }

            /* allocates memory for children */

            if (schema->nb_cols > 0)
            {
                schema->cols = (OCI_Column *) OCI_MemAlloc(OCI_IPC_COLUMN,
                                                           sizeof(*schema->cols),
                                                           schema->nb_cols,
                                                           TRUE);

                /* describe children */

                if (schema->cols != NULL)
                {
                    for (i = 0; i < schema->nb_cols; i++)
                    {
                        res = res && OCI_ColumnDescribe(&schema->cols[i], con, 
                                                        NULL, parmh2, i + 1, ptype);
                        
                        res = res && OCI_ColumnMap(&schema->cols[i], NULL);

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
       OCI_SchemaFree(schema);        
       schema = NULL;
    }

    OCI_RESULT(res);

    return schema; 
}

/* ------------------------------------------------------------------------ *
 * OCI_SchemaFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_SchemaFree(OCI_Schema *schema)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_SCHEMA, schema, FALSE);

    OCI_ListRemove(schema->con->sobjs, schema);

    res = OCI_SchemaClose(schema);

    OCI_FREE(schema);
 
    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_SchemaGetType
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_SchemaGetType(OCI_Schema *schema)
{
    OCI_CHECK_PTR(OCI_IPC_SCHEMA, schema, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return schema->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_SchemaGetColumnCount
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_SchemaGetColumnCount(OCI_Schema *schema)
{
    OCI_CHECK_PTR(OCI_IPC_SCHEMA, schema, 0);
  
    OCI_RESULT(TRUE);

    return schema->nb_cols;
}

/* ------------------------------------------------------------------------ *
 * OCI_SchemaGetColumn
 * ------------------------------------------------------------------------ */

OCI_Column * OCI_API OCI_SchemaGetColumn(OCI_Schema *schema, unsigned int index)
{
    OCI_CHECK_PTR(OCI_IPC_SCHEMA, schema, NULL);
    OCI_CHECK_BOUND(schema->con, index, 1,  schema->nb_cols, NULL);

    OCI_RESULT(TRUE);

    return &(schema->cols[index-1]);
}

/* ------------------------------------------------------------------------ *
 * OCI_SchemaGetName
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_SchemaGetName(OCI_Schema *schema)
{
    OCI_CHECK_PTR(OCI_IPC_SCHEMA, schema, NULL);

    OCI_RESULT(TRUE);

    return schema->name;
}
