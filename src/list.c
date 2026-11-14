/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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

#include "list.h"

#include <stdlib.h>

#include "macros.h"
#include "memory.h"

#define LIST_FOR_EACH(exp)         \
                                   \
        if (list)                  \
        {                          \
            OCI_Item *item = NULL; \
            item = list->head;     \
            while (item)           \
            {                      \
                exp;               \
                item = item->next; \
            }                      \
        }

/* --------------------------------------------------------------------------------------------- *
 * OcilibListCreateItem
 * --------------------------------------------------------------------------------------------- */

static OCI_Item * OcilibListCreateItem
(
    OCI_Environment *env,
    OCI_PointerType  type,
    OCI_Int32        size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Item*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_Item* item = NULL;

    CHECK_BOUND(size, 0, INT_MAX);

    /* allocate list item entry */

    item =  (OCI_Item *)OcilibMemoryAlloc(env, OCI_IPC_LIST_ITEM,
                                          sizeof(*item),
                                          (OCI_Size) 1, TRUE);

    CHECK_NULL(item);

    /* allocate item data buffer */

    item->data = (void *)OcilibMemoryAlloc(env, type, (OCI_Size) size,
                                           (OCI_Size) 1, TRUE);

    CHECK_NULL(item->data);

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            if (NULL != item)
            {
                if (NULL != item->data)
                {
                    FREE(item->data);
                }

                FREE(item);
            }
        }

        SET_RETVAL(item);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibListCreate
 * --------------------------------------------------------------------------------------------- */

OCI_List* OcilibListCreate
(
    OCI_Environment *env,
    OCI_PointerType  type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_List*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    )

    /* allocate list */

    OCI_List *list = (OCI_List *)OcilibMemoryAlloc(env, OCI_IPC_LIST,
                                                   sizeof(*list),
                                                   (OCI_Size) 1, TRUE);

    CHECK_NULL(list);

    /* create a mutex on multi threaded environments */

    list->env   = env;
    list->htype = OCI_IPC_LIST;
    list->type  = type;

    if (LIB_THREADED(env))
    {
        if (mtx_init(&list->mutex, mtx_plain) != thrd_success)
        {
            THROW(OcilibExceptionMemory, OCI_IPC_LIST, 0);
        }
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            FREE(list);
        }

        SET_RETVAL(list);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibListFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibListFree
(
    OCI_List *list
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (list ? list->env : NULL)
    );

    CHECK_PTR(OCI_IPC_LIST, list);

    OcilibListClear(list);

    if (LIB_THREADED(list->env))
    {
        mtx_destroy(&list->mutex);
    }

    OcilibErrorResetSource(NULL, list);

    FREE(list);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibListAppend
 * --------------------------------------------------------------------------------------------- */

void * OcilibListAppend
(
    OCI_List *list,
    OCI_Int32 size
)
{
    ENTER_FUNC
    (
        /* returns */ void*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, (list ? list->env : NULL)
    );

    OCI_Item *item = NULL;
    OCI_Item *temp = NULL;

    CHECK_PTR(OCI_IPC_LIST, list);

    item = OcilibListCreateItem(list->env, list->type, size);
    CHECK_NULL(item);

    temp = list->head;

    while (NULL != temp && NULL != temp->next)
    {
        temp = temp->next;
    }

    if (NULL != temp)
    {
        temp->next = item;
    }
    else
    {
        list->head = item;
    }

    list->count++;

    SET_RETVAL(item->data);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibListClear
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibListClear
(
    OCI_List *list
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (list ? list->env : NULL)
    );

    OCI_Item *item = NULL;

    CHECK_PTR(OCI_IPC_LIST, list);

    /* walk along the list to free item's buffer */

    item = list->head;

    while (NULL != item)
    {
        OCI_Item *temp = item;

        item = item->next;

        /* free data */

        FREE(temp->data);
        FREE(temp);
    }

    list->head  = NULL;
    list->count = 0;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibListForEach
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibListForEach
(
    OCI_List          *list,
    POCI_LIST_FOR_EACH proc
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (list ? list->env : NULL)
    );

    CHECK_PTR(OCI_IPC_LIST, list);
    CHECK_PTR(OCI_IPC_VOID, proc);

    LIST_FOR_EACH(proc(item->data));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibListDisposeAll
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibListDisposeAll
(
    OCI_List         *list,
    POCI_LIST_DISPOSE proc
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (list ? list->env : NULL)
    );

    OCI_Item  *item   = NULL;
    OCI_Item  *prev   = NULL;
    OCI_UInt32 failed = 0;

    CHECK_PTR(OCI_IPC_LIST, list);
    CHECK_PTR(OCI_IPC_VOID, proc);

    /* walk the list and dispose each item; on failure the item is kept in
       the list (with its data) so the failure stays visible and the object
       reference is not silently lost */

    item = list->head;

    while (NULL != item)
    {
        OCI_Item *next = item->next;

        if (proc(item->data))
        {
            FREE(item->data);
            FREE(item);

            if (NULL != prev)
            {
                prev->next = next;
            }
            else
            {
                list->head = next;
            }

            list->count--;
        }
        else
        {
            failed++;

            OCILIB_LOG(OCI_LOG_CAT_LIB, OCI_LOG_WARN,
                       OTEXT("dispose failed for item %p (list type %d); item kept in list"),
                       item->data, (OCI_Int32)list->type);

            prev = item;
        }

        item = next;
    }

    SET_RETVAL(0 == failed);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibListForEachWithParam
* --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibListForEachWithParam
(
    OCI_List                     *list,
    void                         *param,
    POCI_LIST_FOR_EACH_WITH_PARAM proc
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (list ? list->env : NULL)
    );

    CHECK_PTR(OCI_IPC_LIST, list);
    CHECK_PTR(OCI_IPC_VOID, proc);

    LIST_FOR_EACH(proc(item->data, param));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibListRemove
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibListRemove
(
    OCI_List *list,
    void     *data
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (list ? list->env : NULL)
    );

    OCI_Item *item = NULL;
    OCI_Item *temp = NULL;

    CHECK_PTR(OCI_IPC_LIST, list);
    CHECK_PTR(OCI_IPC_VOID, data);

    item = list->head;

    OCI_Boolean found = FALSE;

    while (NULL != item)
    {
        if (item->data == data)
        {
            found = TRUE;

            if (NULL != temp)
            {
                temp->next = item->next;
            }

            /* if item was the first entry, readjust the first list
               entry to next element */

            if (item == list->head)
            {
                list->head = item->next;
            }

            FREE(item);

            break;
        }

        temp = item;
        item = item->next;
    }

    if (found)
    {
        list->count--;
    }

    SET_RETVAL(found);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibListExists
* --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibListExists
(
    OCI_List *list,
    void     *data
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (list ? list->env : NULL)
    );

    CHECK_PTR(OCI_IPC_LIST, list);
    CHECK_PTR(OCI_IPC_VOID, data);

    LIST_FOR_EACH
    (
        if (item->data == data)
        {
            SET_SUCCESS();
            break;
        }
    );

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibListFind
* --------------------------------------------------------------------------------------------- */

void * OcilibListFind
(
    OCI_List      *list,
    POCI_LIST_FIND proc,
    void          *param
)
{
    ENTER_FUNC
    (
        /* returns */ void *, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, (list ? list->env : NULL)
    );

    CHECK_PTR(OCI_IPC_LIST,  list);
    CHECK_PTR(OCI_IPC_VOID, param);

    void* data = NULL;

    LIST_FOR_EACH
    (
        if (proc(item->data, param))
        {
            data = item->data;
            break;
        }
    );

    SET_RETVAL(data);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibListLock
* --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibListLock
(
    OCI_List* list
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (list ? list->env : NULL)
    );

    CHECK_PTR(OCI_IPC_LIST, list);

    if (LIB_THREADED(list->env))
    {
        if (mtx_lock(&list->mutex) != thrd_success)
        {
            THROW_NO_ARGS(OcilibExceptionMutex);
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibListUnlock
* --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibListUnlock
(
    OCI_List* list
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (list ? list->env : NULL)
    );

    CHECK_PTR(OCI_IPC_LIST, list);

    if (LIB_THREADED(list->env))
    {
        if (mtx_unlock(&list->mutex) != thrd_success)
        {
            THROW_NO_ARGS(OcilibExceptionMutex);
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}
