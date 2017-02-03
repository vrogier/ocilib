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
 *                            PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ArrayInit
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ArrayInit
(
    OCI_Array    *arr,
    OCI_TypeInfo *typinf
)
{
    unsigned int i;

    for (i = 0; i < arr->nb_elem; i++)
    {
		void *handle = NULL;

		if (OCI_CDT_DATETIME == arr->elem_type)
        {
            handle = &(((OCIDate *)(arr->mem_handle))[i]);
        }
        else if (OCI_IS_OCI_NUMBER(arr->elem_type, arr->elem_subtype))
        {
            handle = &(((OCINumber *)(arr->mem_handle))[i]);
        }
        else
        {
            handle = ((void **)(arr->mem_handle))[i];
        }

        arr->tab_obj[i] = ((char *) arr->mem_struct) + (arr->struct_size * i);

        ((OCI_Datatype *) (arr->tab_obj[i]))->hstate = OCI_OBJECT_ALLOCATED_ARRAY;

        switch (arr->elem_type)
        {
            case OCI_CDT_NUMERIC:
            {
                if (OCI_NUM_NUMBER == arr->elem_subtype)
                {
                    arr->tab_obj[i] = OCI_NumberInit(arr->con, (OCI_Number *) arr->tab_obj[i], (OCINumber *) handle);
                }
                break;
            }
            case OCI_CDT_DATETIME:
            {
                arr->tab_obj[i] = OCI_DateInit(arr->con, (OCI_Date *) arr->tab_obj[i], (OCIDate *) handle, FALSE, FALSE);
                break;
            }
            case OCI_CDT_LOB:
            {
                arr->tab_obj[i] = OCI_LobInit(arr->con, (OCI_Lob *) arr->tab_obj[i], (OCILobLocator *) handle, arr->elem_subtype);
                break;
            }
            case OCI_CDT_FILE:
            {
                arr->tab_obj[i] = OCI_FileInit(arr->con, (OCI_File *) arr->tab_obj[i], (OCILobLocator *) handle, arr->elem_subtype);
                break;
            }
            case OCI_CDT_TIMESTAMP:
            {
                arr->tab_obj[i] = OCI_TimestampInit(arr->con, (OCI_Timestamp *) arr->tab_obj[i], (OCIDateTime *) handle, arr->elem_subtype);
                break;
            }
            case OCI_CDT_INTERVAL:
            {
                arr->tab_obj[i] = OCI_IntervalInit(arr->con, (OCI_Interval *) arr->tab_obj[i], (OCIInterval *) handle, arr->elem_subtype);
                break;
            }
            case OCI_CDT_OBJECT:
            {
                arr->tab_obj[i] = OCI_ObjectInit(arr->con, (OCI_Object *) arr->tab_obj[i], handle, typinf, NULL, -1, TRUE);
                break;
            }
            case OCI_CDT_COLLECTION:
            {
                arr->tab_obj[i] = OCI_CollInit(arr->con, (OCI_Coll *) arr->tab_obj[i], handle, typinf);
                break;
            }
            case OCI_CDT_REF:
            {
                arr->tab_obj[i] = OCI_RefInit(arr->con, typinf, (OCI_Ref *) arr->tab_obj[i], handle);
                break;
            }
        }
    }

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ArrayClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ArrayClose
(
    OCI_Array *arr
)
{
    OCI_CHECK(NULL == arr, FALSE)

    if (OCI_IS_OCILIB_OBJECT(arr->elem_type, arr->elem_subtype))
    {
		unsigned int i;
		
        /* Cleanup OCILIB Objects */

        for (i = 0; i < arr->nb_elem; i++)
        {
            OCI_FreeObjectFromType(arr->tab_obj[i], arr->elem_type);
        }
    }

    /* free OCI descriptors */

    if (OCI_UNKNOWN != arr->handle_type)
    {
        OCI_DescriptorArrayFree((dvoid **) arr->mem_handle, (ub4) arr->handle_type, (ub4) arr->nb_elem);
    }

    OCI_FREE(arr->mem_handle)
    OCI_FREE(arr->mem_struct)
    OCI_FREE(arr->tab_obj)

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Array * OCI_ArrayCreate
(
    OCI_Connection *con,
    unsigned int    nb_elem,
    unsigned int    elem_type,
    unsigned int    elem_subtype,
    unsigned int    elem_size,
    unsigned int    struct_size,
    unsigned int    handle_type,
    OCI_TypeInfo   *typinf
)
{   
    OCI_Array *arr = NULL;
    OCI_Item  *item = NULL;

    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    /* create array object */

    item = OCI_ListAppend(OCILib.arrs, sizeof(*arr));
    OCI_STATUS = (NULL != item);

    if (OCI_STATUS)
    {
        arr = (OCI_Array *) item->data;

        arr->con          = con;
        arr->elem_type    = elem_type;
        arr->elem_subtype = elem_subtype;
        arr->elem_size    = elem_size;
        arr->nb_elem      = nb_elem;
        arr->struct_size  = struct_size;
        arr->handle_type  = handle_type;

        /* allocate buffers */

        if (OCI_IS_OCILIB_OBJECT(arr->elem_type, arr->elem_subtype))
        {
            OCI_ALLOCATE_DATA(OCI_IPC_VOID, arr->tab_obj, nb_elem)
        }

        OCI_ALLOCATE_BUFFER(OCI_IPC_VOID, arr->mem_handle, elem_size, nb_elem)
        OCI_ALLOCATE_BUFFER(OCI_IPC_VOID, arr->mem_struct, struct_size, nb_elem)

        /* allocate OCI handle descriptors */

        if (OCI_STATUS && handle_type != 0)
        {
            OCI_STATUS = OCI_DescriptorArrayAlloc((dvoid  *)arr->con->env, (dvoid **)arr->mem_handle, (ub4)handle_type, (ub4)nb_elem);
        }

        if (OCI_STATUS && arr->tab_obj && arr->mem_handle)
        {
            OCI_STATUS = OCI_ArrayInit(arr, typinf);
        }
    }

    /* check for failure */

    if (!OCI_STATUS)
    {
        OCI_ArrayClose(arr);
        OCI_FREE(arr)
    }

    return arr;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ArrayFreeFromHandles
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ArrayFreeFromHandles
(
    void **handles
)
{
    boolean    res  = TRUE;
    OCI_List  *list = OCILib.arrs;
    OCI_Item  *item = NULL;
    OCI_Array *arr  = NULL;

    OCI_CHECK(NULL == list, FALSE)

    if (list->mutex)
    {
        OCI_MutexAcquire(list->mutex);
    }

    item = list->head;

    while (item)
    {
        OCI_Array * tmp_arr = (OCI_Array *) item->data;

        if (tmp_arr && ((tmp_arr->tab_obj == handles) || tmp_arr->mem_struct == handles))
        {
            arr = tmp_arr;
            break;
        }

        item = item->next;
    }

    if (list->mutex)
    {
        OCI_MutexRelease(list->mutex);
    }

    if (arr)
    {
        res = OCI_ListRemove(OCILib.arrs, arr);
        OCI_ArrayClose(arr);
        OCI_FREE(arr)
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ArrayGetOCIHandlesFromHandles
 * --------------------------------------------------------------------------------------------- */

void * OCI_ArrayGetOCIHandlesFromHandles
(
    void **handles
)
{
    OCI_List  *list = OCILib.arrs;
    OCI_Item  *item = NULL;
    OCI_Array *arr  = NULL;
    void      *ret  = NULL;

    OCI_CHECK(NULL == list, NULL)

    if (list->mutex)
    {
        OCI_MutexAcquire(list->mutex);
    }

    item = list->head;

    while (item)
    {
        OCI_Array * tmp_arr = (OCI_Array *) item->data;

        if (tmp_arr && (tmp_arr->tab_obj == handles))
        {
            arr = tmp_arr;
            break;
        }

        item = item->next;
    }

    if (list->mutex)
    {
        OCI_MutexRelease(list->mutex);
    }

    if (arr)
    {
        ret = arr->mem_handle;
    }

    return ret;
}
