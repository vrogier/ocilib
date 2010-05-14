/*
   +----------------------------------------------------------------------+
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://www.ocilib.net                 |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2010 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Lesser General Public           |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Lesser General Public License for more details.                      |
   |                                                                      |
   | You should have received a copy of the GNU Lesser General Public     |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@ocilib.net>            |
   +----------------------------------------------------------------------+
*/

/* ------------------------------------------------------------------------ *
 * $Id: array.c, v 3.6.0 2010-05-14 20:21 Vincent Rogier $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                            PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ArrayGetDatatypeInfo
 * ------------------------------------------------------------------------ */

boolean OCI_ArrayGetDatatypeInfo(unsigned int type,
                                 unsigned int subtype,
                                 unsigned int *handle_type,
                                 size_t *struct_size,
                                 size_t *handle_size)
{
    *handle_size = sizeof(void *);

    switch (type)
    {
        case OCI_CDT_DATETIME :

            *struct_size = sizeof(OCI_Date);
            *handle_size = sizeof(OCIDate);
            *handle_type = 0;

            break;

        case OCI_CDT_LOB:

            *struct_size = sizeof(OCI_Lob);
            *handle_type = OCI_DTYPE_LOB;

            break;

        case OCI_CDT_FILE:

            *struct_size = sizeof(OCI_File);
            *handle_type = OCI_DTYPE_LOB;

            break;

       case OCI_CDT_TIMESTAMP:

            *struct_size = sizeof(OCI_Timestamp);

            if (subtype == OCI_TIMESTAMP)
            {
                *handle_type = OCI_DTYPE_TIMESTAMP;
            }
            else if (subtype == OCI_TIMESTAMP_TZ)
            {
                *handle_type = OCI_DTYPE_TIMESTAMP_TZ;
            }
            else if (subtype == OCI_TIMESTAMP_LTZ)
            {
                *handle_type = OCI_DTYPE_TIMESTAMP_LTZ;
            }

            break;

        case OCI_CDT_INTERVAL:

            *struct_size = sizeof(OCI_Interval);

            if (subtype == OCI_INTERVAL_YM)
            {
                *handle_type = OCI_DTYPE_INTERVAL_YM;
            }
            else if (subtype == OCI_INTERVAL_DS)
            {
                *handle_type = OCI_DTYPE_INTERVAL_DS;
            }

            break;

        case OCI_CDT_OBJECT:

            *struct_size = sizeof(OCI_Object);
            *handle_type = 0;

            break;

        case OCI_CDT_COLLECTION:

            *struct_size = sizeof(OCI_Coll);
            *handle_type = 0;

            break;

        case OCI_CDT_REF:

            *struct_size = sizeof(OCI_Ref);
            *handle_type = 0;

            break;

    }

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ArrayInit
 * ------------------------------------------------------------------------ */

boolean OCI_ArrayInit(OCI_Array *arr, size_t struct_size, size_t handle_size,
                      OCI_TypeInfo *typinf)
{
    unsigned int i;

    OCI_NOT_USED(handle_size);

    for (i = 0; i < arr->nb_elem; i++)
    {
        void *handle = NULL;

        if (arr->type == OCI_CDT_DATETIME)
            handle = &(((OCIDate *)(arr->mem_handle))[i]);
        else
            handle = ((void **)(arr->mem_handle))[i];

        arr->tab_obj[i] = ((char *) arr->mem_struct) + (struct_size * i);

        ((OCI_Datatype *) (arr->tab_obj[i]))->hstate = OCI_OBJECT_ALLOCATED_ARRAY;

        switch (arr->type)
        {
            case OCI_CDT_DATETIME :

                OCI_DateInit(arr->con, (OCI_Date **) &arr->tab_obj[i],
                             handle, FALSE, FALSE);

                break;

            case OCI_CDT_LOB:

                OCI_LobInit(arr->con, (OCI_Lob **) &arr->tab_obj[i],
                            handle, arr->subtype);

                break;

            case OCI_CDT_FILE:

                OCI_FileInit(arr->con, (OCI_File **) &arr->tab_obj[i],
                             handle, arr->subtype);
                break;

            case OCI_CDT_TIMESTAMP:

                OCI_TimestampInit(arr->con, (OCI_Timestamp **) &arr->tab_obj[i],
                                  handle, arr->subtype);

                break;

            case OCI_CDT_INTERVAL:

                OCI_IntervalInit(arr->con, (OCI_Interval **) &arr->tab_obj[i],
                                 handle, arr->subtype);

                break;

            case OCI_CDT_OBJECT:

                OCI_ObjectInit(arr->con, (OCI_Object **) &arr->tab_obj[i],
                               handle, typinf, NULL, -1, TRUE);

                break;

            case OCI_CDT_COLLECTION:

                OCI_CollInit(arr->con, (OCI_Coll **) &arr->tab_obj[i],
                             handle, typinf);

                break;

            case OCI_CDT_REF:

                OCI_RefInit(arr->con, typinf, (OCI_Ref **) &arr->tab_obj[i], handle);

                break;
        }
    }

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ArrayCleanup
 * ------------------------------------------------------------------------ */

boolean OCI_ArrayClose(OCI_Array *arr)
{
    unsigned int i;
    size_t struct_size       = 0;
    size_t handle_size       = 0;
    unsigned int handle_type = 0;

    OCI_CHECK_PTR(OCI_IPC_ARRAY, arr, FALSE);

    /* free array structure */

    OCI_ArrayGetDatatypeInfo(arr->type, arr->subtype, &handle_type,
                             &struct_size, &handle_size);

    if (handle_type != 0)
    {
        OCI_DescriptorArrayFree
        (
            (dvoid **) arr->mem_handle,
            (ub4     ) handle_type,
            (ub4     ) arr->nb_elem
        );
    }

    for (i = 0; i < arr->nb_elem; i++)
    {
        switch (arr->type)
        {
            case OCI_CDT_DATETIME :

                OCI_DateFree((OCI_Date *) arr->tab_obj[i]);

                break;

            case OCI_CDT_LOB:

                OCI_LobFree((OCI_Lob *) arr->tab_obj[i]);
                break;

            case OCI_CDT_FILE:

                OCI_FileFree((OCI_File *) arr->tab_obj[i]);
                break;

            case OCI_CDT_TIMESTAMP:

                OCI_TimestampFree((OCI_Timestamp *) arr->tab_obj[i]);

                break;

            case OCI_CDT_INTERVAL:

                OCI_IntervalFree((OCI_Interval *) arr->tab_obj[i]);

                break;

            case OCI_CDT_OBJECT:

                OCI_ObjectFree((OCI_Object *) arr->tab_obj[i]);

                break;

            case OCI_CDT_COLLECTION:

                OCI_CollFree((OCI_Coll *) arr->tab_obj[i]);

                break;

            case OCI_CDT_REF:

                OCI_RefFree((OCI_Ref *) arr->tab_obj[i]);
                break;

        }
    }

    OCI_FREE(arr->mem_handle);
    OCI_FREE(arr->mem_struct);
    OCI_FREE(arr->tab_obj);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ArrayCreate
 * ------------------------------------------------------------------------ */

OCI_Array * OCI_ArrayCreate(OCI_Connection *con, unsigned int nb_elem,
                                    unsigned int type, unsigned int subtype,
                                    OCI_TypeInfo *typinf)
{
    boolean res              = TRUE;
    OCI_Array *arr           = NULL;
    size_t struct_size       = 0;
    size_t handle_size       = 0;
    unsigned int handle_type = 0;
    OCI_Item *item           = NULL;

    OCI_CHECK_INITIALIZED(NULL);
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    /* create connection pool object */

    item = OCI_ListAppend(OCILib.arrs, sizeof(*arr));

    if (item != NULL)
    {
        arr = (OCI_Array *) item->data;

        arr->con        = con;
        arr->type       = type;
        arr->subtype    = subtype;
        arr->nb_elem    = nb_elem;

        OCI_ArrayGetDatatypeInfo(type, subtype, &handle_type,
                                 &struct_size, &handle_size);

        arr->tab_obj    = (void **) OCI_MemAlloc(OCI_IPC_VOID, sizeof(void *), nb_elem, TRUE);
        arr->mem_handle = (void **) OCI_MemAlloc(OCI_IPC_VOID, handle_size, nb_elem, TRUE);
        arr->mem_struct = (void  *) OCI_MemAlloc(OCI_IPC_VOID, struct_size, nb_elem, TRUE);

        res = ((arr->tab_obj != NULL) && (arr->mem_handle != NULL) && (arr->mem_struct != NULL));

        if (res == TRUE)
        {
            if (handle_type != 0)
            {
               res = (OCI_SUCCESS == OCI_DescriptorArrayAlloc
                                     (
                                         (dvoid  *) OCILib.env,
                                         (dvoid **) arr->mem_handle,
                                         (ub4     ) handle_type,
                                         (ub4     ) nb_elem,
                                         (size_t  ) 0,
                                         (dvoid **) NULL
                                     ));
            }
        }

        if (res == TRUE)
        {
            res = OCI_ArrayInit(arr, struct_size, handle_size, typinf);
        }
    }
    else
    {
        res = FALSE;
    }

    /* check for failure */

    if (res == FALSE)
    {
        OCI_ArrayClose(arr);
        OCI_FREE(arr);
    }

    OCI_RESULT(res);

    return arr;
}

/* ------------------------------------------------------------------------ *
 * OCI_ArrayFreeFromHandles
 * ------------------------------------------------------------------------ */

boolean OCI_ArrayFreeFromHandles(void ** handles)
{
    boolean res     = FALSE;
    OCI_List *list  = OCILib.arrs;
    OCI_Item *item  = NULL;
    OCI_Array *arr  = NULL;

    OCI_CHECK_PTR(OCI_IPC_VOID, handles, FALSE);

    OCI_CHECK(list == NULL, FALSE);

    if (list->mutex != NULL)
        OCI_MutexAcquire(list->mutex);

    item = list->head;

    while (item != NULL)
    {
        OCI_Array * tmp_arr = (OCI_Array *) item->data;

        if ((tmp_arr != NULL) && (tmp_arr->tab_obj == handles))
        {
            arr = tmp_arr;
            break;
        }

        item = item->next;
    }

    if (list->mutex != NULL)
        OCI_MutexRelease(list->mutex);

    if (arr != NULL)
    {
         res = OCI_ListRemove(OCILib.arrs, arr);
         OCI_ArrayClose(arr);
         OCI_FREE(arr);
   }

    OCI_RESULT(res);

    return res;
}
