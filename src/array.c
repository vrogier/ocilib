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

#include "array.h"

#include "collection.h"
#include "date.h"
#include "file.h"
#include "helpers.h"
#include "interval.h"
#include "list.h"
#include "lob.h"
#include "macro.h"
#include "memory.h"
#include "mutex.h"
#include "number.h"
#include "object.h"
#include "ref.h"
#include "timestamp.h"

#define OCI_ARRAY_INIT_HANDLE(type, func)                                   \
    arr->tab_obj[i] = func;                                                 \
    ((void **)(arr->mem_handle))[i] = ((type *) arr->tab_obj[i])->handle;   \

/* --------------------------------------------------------------------------------------------- *
 * ArrayFindAny
 * --------------------------------------------------------------------------------------------- */

boolean ArrayFindAny
(
    OCI_Array *arr,
    void     **handles
)
{
    return arr && (arr->tab_obj == handles || arr->mem_struct == handles);
}

/* --------------------------------------------------------------------------------------------- *
* ArrayFindObjects
* --------------------------------------------------------------------------------------------- */

boolean ArrayFindObjects
(
    OCI_Array *arr,
    void     **handles
)

{
    return arr && arr->tab_obj == handles;
}

/* --------------------------------------------------------------------------------------------- *
 * _ArrayInit
 * --------------------------------------------------------------------------------------------- */

boolean ArrayInit
(
    OCI_Array    *arr,
    OCI_TypeInfo *typinf
)
{
    for (unsigned int i = 0; i < arr->nb_elem; i++)
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
                    arr->tab_obj[i] = NumberInit(arr->con, (OCI_Number *) arr->tab_obj[i], (OCINumber *) handle);
                }
                break;
            }
            case OCI_CDT_DATETIME:
            {
                arr->tab_obj[i] = DateInit(arr->con, (OCI_Date *) arr->tab_obj[i], (OCIDate *) handle, FALSE, FALSE);
                break;
            }
            case OCI_CDT_LOB:
            {
                OCI_ARRAY_INIT_HANDLE(OCI_Lob, LobInit(arr->con, (OCI_Lob *) arr->tab_obj[i], (OCILobLocator *) handle, arr->elem_subtype))
                break;
            }
            case OCI_CDT_FILE:
            {
                OCI_ARRAY_INIT_HANDLE(OCI_File, FileInit(arr->con, (OCI_File *) arr->tab_obj[i], (OCILobLocator *) handle, arr->elem_subtype))
                break;
            }
            case OCI_CDT_TIMESTAMP:
            {
                OCI_ARRAY_INIT_HANDLE(OCI_Timestamp, TimestampInit(arr->con, (OCI_Timestamp *) arr->tab_obj[i], (OCIDateTime *) handle, arr->elem_subtype))
                break;
            }
            case OCI_CDT_INTERVAL:
            {
                OCI_ARRAY_INIT_HANDLE(OCI_Interval, IntervalInit(arr->con, (OCI_Interval *) arr->tab_obj[i], (OCIInterval *) handle, arr->elem_subtype))
                break;
            }
            case OCI_CDT_OBJECT:
            {
                OCI_ARRAY_INIT_HANDLE(OCI_Object, ObjectInit(arr->con, (OCI_Object *)arr->tab_obj[i], handle, typinf, NULL, -1, TRUE))
                break;
            }
            case OCI_CDT_COLLECTION:
            {
                OCI_ARRAY_INIT_HANDLE(OCI_Coll, CollInit(arr->con, (OCI_Coll *) arr->tab_obj[i], handle, typinf))
                break;
            }
            case OCI_CDT_REF:
            {
                OCI_ARRAY_INIT_HANDLE(OCI_Ref, RefInit(arr->con, typinf, (OCI_Ref *) arr->tab_obj[i], handle))
                break;
            }
        }
    }

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * ArrayClose
 * --------------------------------------------------------------------------------------------- */

boolean ArrayClose
(
    OCI_Array *arr
)
{
    OCI_CHECK(NULL == arr, FALSE)

    if (OCI_IS_OCILIB_OBJECT(arr->elem_type, arr->elem_subtype))
    {
        /* Cleanup OCILIB Objects */

        for (unsigned int i = 0; i < arr->nb_elem; i++)
        {
            FreeObjectFromType(arr->tab_obj[i], arr->elem_type);
        }
    }

    /* free OCI descriptors */

    if (OCI_UNKNOWN != arr->handle_type)
    {
        MemDescriptorArrayFree((dvoid **) arr->mem_handle, (ub4) arr->handle_type, (ub4) arr->nb_elem);
    }

    OCI_FREE(arr->mem_handle)
    OCI_FREE(arr->mem_struct)
    OCI_FREE(arr->tab_obj)

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * ArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Array * ArrayCreate
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

    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    /* create array object */

    arr = ListAppend(OCILib.arrs, sizeof(*arr));
    OCI_STATUS = (NULL != arr);

    if (OCI_STATUS)
    {
        arr->con          = con;
        arr->err          = con ? con->err : OCILib.err;
        arr->env          = con ? con->env : OCILib.env;
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
            OCI_STATUS = MemDescriptorArrayAlloc((dvoid  *)arr->env, (dvoid **)arr->mem_handle, (ub4)handle_type, (ub4)nb_elem);
        }

        if (OCI_STATUS && arr->tab_obj && arr->mem_handle)
        {
            OCI_STATUS = ArrayInit(arr, typinf);
        }
    }

    /* check for failure */

    if (!OCI_STATUS)
    {
        ArrayClose(arr);
        OCI_FREE(arr)
    }

    return arr;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ArrayFreeFromHandles
 * --------------------------------------------------------------------------------------------- */

boolean ArrayFreeFromHandles
(
    void **handles
)
{
    boolean    res = FALSE;
    OCI_Array *arr = ListFind(OCILib.arrs, (POCI_LIST_FIND) ArrayFindAny, handles);

    if (arr)
    {
        res = ListRemove(OCILib.arrs, arr);
       ArrayClose(arr);
        OCI_FREE(arr)
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * ArrayGetOCIHandlesFromHandles
 * --------------------------------------------------------------------------------------------- */

void * ArrayGetOCIHandlesFromHandles
(
    void **handles
)
{
    void      *ret = NULL;
    OCI_Array *arr = ListFind(OCILib.arrs, (POCI_LIST_FIND) ArrayFindObjects, handles);

    if (arr)
    {
        ret = arr->mem_handle;
    }

    return ret;
}
