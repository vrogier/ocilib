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

#include "array.h"

#include "collection.h"
#include "date.h"
#include "file.h"
#include "helpers.h"
#include "interval.h"
#include "json.h"
#include "list.h"
#include "lob.h"
#include "macros.h"
#include "memory.h"
#include "number.h"
#include "object.h"
#include "reference.h"
#include "timestamp.h"
#include "vector.h"

#define ARRAY_INIT(type, exp)                                      \
        data = exp;                                                \
        CHECK_NULL(data);                                          \
        ((void **)(arr->mem_handle))[i] = ((type *) data)->handle; \


/* --------------------------------------------------------------------------------------------- *
 * OcilibArrayFindAny
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibArrayFindAny
(
    OCI_Array *arr,
    void     **handles
)
{
    return arr && (arr->tab_obj == handles || arr->mem_struct == handles);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibArrayInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibArrayInitialize
(
    OCI_Array    *arr,
    OCI_TypeInfo *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ARRAY, arr
    );

    for (OCI_UInt32 i = 0; i < arr->nb_elem; i++)
    {
        void *handle = NULL;
        void *data   = ((char*)arr->mem_struct) + (arr->struct_size * i);;

        if (OCI_CDT_DATETIME == arr->elem_type)
        {
            handle = &(((OCIDate *)(arr->mem_handle))[i]);
        }
        else if (IS_OCI_NUMBER(arr->elem_type, arr->elem_subtype))
        {
            handle = &(((OCINumber *)(arr->mem_handle))[i]);
        }
        else
        {
            handle = ((void **)(arr->mem_handle))[i];
        }

        arr->tab_obj[i] = data;

        ((OCI_DataType *) data)->hstate = OCI_OBJECT_ALLOCATED_ARRAY;

        switch (arr->elem_type)
        {
            case OCI_CDT_NUMERIC:
            {
                if (OCI_NUM_NUMBER == arr->elem_subtype)
                {
                    data = OcilibNumberInitialize(arr->env, (OCI_Number*)data, (OCINumber*)handle);
                    CHECK_NULL(data);
                }
                break;
            }
            case OCI_CDT_DATETIME:
            {
                data = OcilibDateInitialize(arr->env, (OCI_Date*)data, (OCIDate*)handle, FALSE, FALSE);
                CHECK_NULL(data);
                break;
            }
            case OCI_CDT_LOB:
            {
                ARRAY_INIT(OCI_Lob, OcilibLobInitialize(arr->con, (OCI_Lob *)data,  (OCILobLocator *) handle, arr->elem_subtype));
                break;
            }
            case OCI_CDT_FILE:
            {
                ARRAY_INIT(OCI_File, OcilibFileInitialize(arr->con, (OCI_File *)data, (OCILobLocator *) handle, arr->elem_subtype));
                break;
            }
            case OCI_CDT_TIMESTAMP:
            {
                ARRAY_INIT(OCI_Timestamp,
                           OcilibTimestampInitialize(arr->env, arr->con, (OCI_Timestamp *)data, (OCIDateTime *) handle, arr->elem_subtype));
                break;
            }
            case OCI_CDT_INTERVAL:
            {
                ARRAY_INIT(OCI_Interval,
                           OcilibIntervalInitialize(arr->env, arr->con, (OCI_Interval *)data, (OCIInterval *) handle, arr->elem_subtype));
                break;
            }
            case OCI_CDT_OBJECT:
            {
                ARRAY_INIT(OCI_Object, OcilibObjectInitialize(arr->con, (OCI_Object *)data, handle, typinf, NULL, -1, TRUE));
                break;
            }
            case OCI_CDT_COLLECTION:
            {
                ARRAY_INIT(OCI_Coll, OcilibCollectionInitialize(arr->con, (OCI_Coll *)data, handle, typinf));
                break;
            }
            case OCI_CDT_REF:
            {
                ARRAY_INIT(OCI_Ref, OcilibReferenceInitialize(arr->con, typinf, (OCI_Ref *) data, handle));
                break;
            }
            case OCI_CDT_VECTOR:
            {
                ARRAY_INIT(OCI_Vector, OcilibVectorInitialize(arr->con,  (OCI_Vector *) data, handle));
                break;
            }
            case OCI_CDT_JSON:
            {
                ARRAY_INIT(OCI_Json, OcilibJsonInitialize(arr->con, (OCI_Json *)data, (OCIJson *) handle));
                break;
            }
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibArrayClose
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibArrayDispose
(
    OCI_Array *arr
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, arr ? arr->env : NULL
    );

    CHECK_PTR(OCI_IPC_ARRAY, arr);

    if (IS_OCILIB_OBJECT(arr->elem_type, arr->elem_subtype))
    {
        /* Cleanup OCILIB Objects */

        for (OCI_UInt32 i = 0; i < arr->nb_elem; i++)
        {
            OcilibFreeObjectFromType(arr->tab_obj[i], arr->elem_type);
        }
    }

    /* free OCI descriptors */

    if (OCI_UNKNOWN != arr->handle_type)
    {
        OcilibMemoryFreeDescriptorArray
        (
            arr->env,
            (dvoid**)arr->mem_handle,
            (ub4)arr->handle_type,
            (ub4)arr->nb_elem
        );
    }

    FREE(arr->mem_handle);
    FREE(arr->mem_struct);
    FREE(arr->tab_obj);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Array * OcilibArrayCreate
(
    OCI_Environment *env,
    OCI_Connection  *con,
    OCI_UInt32       nb_elem,
    OCI_UInt32       elem_type,
    OCI_UInt32       elem_subtype,
    OCI_UInt32       elem_size,
    OCI_UInt32       struct_size,
    OCI_UInt32       handle_type,
    OCI_TypeInfo    *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Array*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_Array* arr = NULL;

    /* create array object */

    LIST_ATOMIC_ADD(env->arrs, arr);

    CHECK_NULL(arr);

    arr->con          = con;
    arr->env          = env;
    arr->elem_type    = elem_type;
    arr->elem_subtype = elem_subtype;
    arr->elem_size    = elem_size;
    arr->nb_elem      = nb_elem;
    arr->struct_size  = struct_size;
    arr->handle_type  = handle_type;

    /* allocate buffers */

    if (IS_OCILIB_OBJECT(arr->elem_type, arr->elem_subtype))
    {
        ALLOC_DATA(OCI_IPC_VOID, arr->tab_obj, nb_elem);
    }

    ALLOC_BUFFER(OCI_IPC_VOID, arr->mem_handle, elem_size,   nb_elem);
    ALLOC_BUFFER(OCI_IPC_VOID, arr->mem_struct, struct_size, nb_elem);

    /* allocate OCI handle descriptors */

    if (OCI_UNKNOWN != handle_type)
    {
        CHECK
        (
            OcilibMemoryAllocDescriptorArray
            (
                arr->env,
                (dvoid  *)arr->env->handle,
                (dvoid **)arr->mem_handle,
                (ub4)handle_type, (ub4)nb_elem
            )
        );
    }

    if (arr->tab_obj && arr->mem_handle)
    {
        CHECK(OcilibArrayInitialize(arr, typinf));
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibArrayDispose(arr);
            arr = NULL;
        }

        SET_RETVAL(arr);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibArrayFreeFromHandles
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibArrayFreeFromHandles
(
    OCI_Environment *env,
    void           **handles
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_Array* arr = NULL;

    CHECK_PTR(OCI_IPC_VOID, handles);

    LIST_ATOMIC_OPERATION
    (
        env->arrs,
        {
            arr = (OCI_Array *) OcilibListFind(env->arrs, (POCI_LIST_FIND) OcilibArrayFindAny, (void *) handles);

            if (NULL != arr)
            {
                OcilibListRemove(env->arrs, arr);
            }
        }
    );

    CHECK_NULL(arr);

    OcilibArrayDispose(arr);
    FREE(arr);

    SET_SUCCESS();

    EXIT_FUNC();
}
