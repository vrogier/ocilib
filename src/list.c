/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
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


#define LIST_FOR_EACH(exp) \
\
    if (list)                               \
    {                                       \
        OCI_Item *item = NULL;              \
        if (list->mutex )                   \
        {                                   \
            OCI_MutexAcquire(list->mutex);  \
        }                                   \
        item = list->head;                  \
        while (item)                        \
        {                                   \
            exp;                            \
            item = item->next;              \
        }                                   \
        if (list->mutex)                    \
        {                                   \
            OCI_MutexRelease(list->mutex);  \
        }                                   \
    }                                       \


/* ********************************************************************************************* *
 *                             LOCAL FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ListCreateItem
 * --------------------------------------------------------------------------------------------- */

OCI_Item * OCI_ListCreateItem
(
    int type,
    int size
)
{
    OCI_Item *item = NULL;

    /* allocate list item entry */

    item = (OCI_Item *) OCI_MemAlloc(OCI_IPC_LIST_ITEM, sizeof(*item), (size_t) 1, TRUE);

    if (item)
    {
        /* allocate item data buffer */

        item->data = (void *) OCI_MemAlloc(type, (size_t) size, (size_t) 1, TRUE);

        if (!item->data)
        {
            OCI_FREE(item)
        }
    }

    return item;
}

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ListCreate
 * --------------------------------------------------------------------------------------------- */

OCI_List * OCI_ListCreate
(
    int type
)
{
    OCI_List *list = NULL;

    /* allocate list */

    list = (OCI_List *) OCI_MemAlloc(OCI_IPC_LIST, sizeof(*list), (size_t) 1, TRUE);

    /* create a mutex on multi threaded environments */

    if (list)
    {
        list->type = type;

        if (OCI_LIB_THREADED)
        {
            list->mutex = OCI_MutexCreateInternal();

            if (!list->mutex)
            {
                OCI_FREE(list)
            }
        }
    }

    return list;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ListFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ListFree
(
    OCI_List *list
)
{
    boolean res = TRUE;

    OCI_CHECK(NULL == list,  FALSE)

    OCI_ListClear(list);

    if (list->mutex)
    {
        res = OCI_MutexFree(list->mutex);
    }

    OCI_FREE(list)

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ListAppend
 * --------------------------------------------------------------------------------------------- */

void * OCI_ListAppend
(
    OCI_List *list,
    int       size
)
{
    OCI_Item *item = NULL;
    OCI_Item *temp = NULL;

    OCI_CHECK(NULL == list,  NULL);

    item = OCI_ListCreateItem(list->type, size);

    OCI_CHECK(NULL == item, FALSE)

    if (list->mutex)
    {
        OCI_MutexAcquire(list->mutex);
    }

    temp = list->head;

    while (temp && temp->next)
    {
        temp = temp->next;
    }

    if (temp)
    {
        temp->next = item;
    }
    else
    {
        list->head = item;
    }

    list->count++;

    if (list->mutex)
    {
        OCI_MutexRelease(list->mutex);
    }

    return item->data;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ListClear
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ListClear
(
    OCI_List *list
)
{
    OCI_Item *item = NULL;

    OCI_CHECK(NULL == list,  FALSE)

    if (list->mutex)
    {
        OCI_MutexAcquire(list->mutex);
    }

    /* walk along the list to free item's buffer */

    item = list->head;

    while (item)
    {
		OCI_Item *temp = item;
		
        item = item->next;

        /* free data */

        OCI_FREE(temp->data)
        OCI_FREE(temp)
    }

    list->head  = NULL;
    list->count = 0;

    if (list->mutex)
    {
        OCI_MutexRelease(list->mutex);
    }

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ListForEach
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ListForEach
(
    OCI_List          *list,
    POCI_LIST_FOR_EACH proc
)
{
    OCI_CHECK(NULL == list, FALSE)
    OCI_CHECK(NULL == proc, FALSE)

    LIST_FOR_EACH(proc(item->data))

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ListForEachWithParam
* --------------------------------------------------------------------------------------------- */

boolean OCI_ListForEachWithParam
(
    OCI_List          *list,
    void              *param,
    POCI_LIST_FOR_EACH_WITH_PARAM proc
)
{
    OCI_CHECK(NULL == list, FALSE)
    OCI_CHECK(NULL == proc, FALSE)

    LIST_FOR_EACH(proc(item->data, param))

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ListRemove
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ListRemove
(
    OCI_List *list,
    void     *data
)
{
    boolean   found = FALSE;
    OCI_Item *item  = NULL;
    OCI_Item *temp  = NULL;

    OCI_CHECK(NULL == list,  FALSE)
    OCI_CHECK(NULL == data, FALSE)

    if (list->mutex)
    {
        OCI_MutexAcquire(list->mutex);
    }

    item = list->head;

    while (item)
    {
        if (item->data == data)
        {
            found = TRUE;

            if (temp)
            {
                temp->next = item->next;
            }

            /* if item was the first entry, readjust the first list
               entry to next element */

            if (item == list->head)
            {
                list->head = item->next;
            }

            OCI_FREE(item)

            break;
        }

        temp = item;
        item = item->next;
    }

    if (found)
    {
        list->count--;
    }

    if (list->mutex)
    {
        OCI_MutexRelease(list->mutex);
    }

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ListExists
* --------------------------------------------------------------------------------------------- */

boolean OCI_ListExists
(
    OCI_List *list,
    void     *data
)
{
    boolean found = FALSE;

    OCI_CHECK(NULL == list, FALSE)

    LIST_FOR_EACH
    (
        if (item->data == data)
        {
            found = TRUE;
            break;
        }
    )

    return found;
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ListFind
* --------------------------------------------------------------------------------------------- */

void * OCI_ListFind
(
    OCI_List        *list,
    POCI_LIST_FIND   proc,
    void            *param
)
{
    void * result = NULL;

    OCI_CHECK(NULL == list, NULL)
    OCI_CHECK(NULL == proc, NULL)

    LIST_FOR_EACH
    (
        if (proc(item->data, param))
        {
            result = item->data;
            break;
        }
    )

    return result;
}
