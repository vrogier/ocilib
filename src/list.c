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

#include "list.h"

#include "macros.h"
#include "memory.h"
#include "mutex.h"

#define ACQUIRE_LOCK()                   \
                                         \
    if (NULL != list->mutex)             \
    {                                    \
        CHECK(MutexAcquire(list->mutex)) \
    }

#define RELEASE_LOCK()                   \
                                         \
    if (NULL != list->mutex)             \
    {                                    \
        CHECK(MutexRelease(list->mutex)) \
    }

#define LIST_FOR_EACH(exp)     \
                               \
    if (list)                  \
    {                          \
        OCI_Item *item = NULL; \
        ACQUIRE_LOCK()         \
        item = list->head;     \
        while (item)           \
        {                      \
            exp;               \
            item = item->next; \
        }                      \
        RELEASE_LOCK()         \
    }

OCI_Item * ListCreateItem
(
    int type,
    int size
);

/* --------------------------------------------------------------------------------------------- *
 * ListCreateItem
 * --------------------------------------------------------------------------------------------- */

OCI_Item * ListCreateItem
(
    int type,
    int size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Item*, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    OCI_Item* item = NULL;

    CHECK_BOUND(size, 0, INT_MAX)

    /* allocate list item entry */

    item =  (OCI_Item *) MemoryAlloc(OCI_IPC_LIST_ITEM,
                                     sizeof(*item),
                                     (size_t) 1, TRUE);

    CHECK_NULL(item)

    /* allocate item data buffer */

    item->data = (void *) MemoryAlloc(type, (size_t) size,
                                      (size_t) 1, TRUE);

    CHECK_NULL(item->data)

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            if (NULL != item->data)
            {
                FREE(item->data)
            }

            if (NULL != item)
            {
                FREE(item)
            }
        }

        SET_RETVAL(item)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ListCreate
 * --------------------------------------------------------------------------------------------- */

OCI_List* ListCreate
(
    int type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_List*, NULL,
        /* context */ OCI_IPC_VOID, &Env
    )

    /* allocate list */

    OCI_List *list = (OCI_List *) MemoryAlloc(OCI_IPC_LIST,
                                              sizeof(*list),
                                              (size_t) 1, TRUE);

    CHECK_NULL(list)

    /* create a mutex on multi threaded environments */

    list->type = type;

    if (LIB_THREADED)
    {
        list->mutex = MutexCreateInternal();
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            FREE(list)
        }

        SET_RETVAL(list)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ListFree
 * --------------------------------------------------------------------------------------------- */

boolean ListFree
(
    OCI_List *list
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LIST, list
    )

    CHECK_PTR(OCI_IPC_LIST, list)

    ListClear(list);

    if (NULL!= list->mutex)
    {
        MutexFree(list->mutex);
    }

    ErrorResetSource(NULL, list);

    FREE(list)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ListAppend
 * --------------------------------------------------------------------------------------------- */

void * ListAppend
(
    OCI_List *list,
    int       size
)
{
    ENTER_FUNC
    (
        /* returns */ void*, NULL,
        /* context */ OCI_IPC_LIST, list
    )

    OCI_Item *item = NULL;
    OCI_Item *temp = NULL;

    CHECK_PTR(OCI_IPC_LIST, list)

    item = ListCreateItem(list->type, size);
    CHECK_NULL(item)

    ACQUIRE_LOCK()

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

    RELEASE_LOCK()

    SET_RETVAL(item->data)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ListClear
 * --------------------------------------------------------------------------------------------- */

boolean ListClear
(
    OCI_List *list
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LIST, list
    )

    OCI_Item *item = NULL;

    CHECK_PTR(OCI_IPC_LIST, list)

    ACQUIRE_LOCK()

    /* walk along the list to free item's buffer */

    item = list->head;

    while (NULL != item)
    {
        OCI_Item *temp = item;

        item = item->next;

        /* free data */

        FREE(temp->data)
        FREE(temp)
    }

    list->head  = NULL;
    list->count = 0;

    RELEASE_LOCK()

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ListForEach
 * --------------------------------------------------------------------------------------------- */

boolean ListForEach
(
    OCI_List          *list,
    POCI_LIST_FOR_EACH proc
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LIST, list
    )

    CHECK_PTR(OCI_IPC_LIST, list)
    CHECK_PTR(OCI_IPC_VOID, proc)

    LIST_FOR_EACH(proc(item->data))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* ListForEachWithParam
* --------------------------------------------------------------------------------------------- */

boolean ListForEachWithParam
(
    OCI_List                     *list,
    void                         *param,
    POCI_LIST_FOR_EACH_WITH_PARAM proc
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LIST, list
    )

    CHECK_PTR(OCI_IPC_LIST, list)
    CHECK_PTR(OCI_IPC_VOID, proc)

    LIST_FOR_EACH(proc(item->data, param))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ListRemove
 * --------------------------------------------------------------------------------------------- */

boolean ListRemove
(
    OCI_List *list,
    void     *data
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LIST, list
    )

    OCI_Item *item = NULL;
    OCI_Item *temp = NULL;

    CHECK_PTR(OCI_IPC_LIST, list)
    CHECK_PTR(OCI_IPC_VOID, data)

    ACQUIRE_LOCK()

    item = list->head;

    boolean found = FALSE;

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

            FREE(item)

            break;
        }

        temp = item;
        item = item->next;
    }

    if (found)
    {
        list->count--;
    }

    RELEASE_LOCK()

    SET_RETVAL(found)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* ListExists
* --------------------------------------------------------------------------------------------- */

boolean ListExists
(
    OCI_List *list,
    void     *data
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LIST, list
    )

    CHECK_PTR(OCI_IPC_LIST, list)
    CHECK_PTR(OCI_IPC_VOID, data)

    LIST_FOR_EACH
    (
        if (item->data == data)
        {
            SET_SUCCESS()
            break;
        }
    )

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* ListFind
* --------------------------------------------------------------------------------------------- */

void * ListFind
(
    OCI_List      *list,
    POCI_LIST_FIND proc,
    void          *param
)
{
    ENTER_FUNC
    (
        /* returns */ void *, NULL,
        /* context */ OCI_IPC_LIST, list
    )

    CHECK_PTR(OCI_IPC_LIST,  list)
    CHECK_PTR(OCI_IPC_VOID, param)

    void* data = NULL;

    LIST_FOR_EACH
    (
        if (proc(item->data, param))
        {
            data = item->data;
            break;
        }
    )

    SET_RETVAL(data)

    EXIT_FUNC()
}
