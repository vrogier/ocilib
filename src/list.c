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
 * $Id: list.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ListCreate
 * ------------------------------------------------------------------------ */

OCI_List * OCI_ListCreate(int type)
{
    OCI_List *list = NULL;
    
    /* allocate list */

    list = (OCI_List *) OCI_MemAlloc(OCI_IPC_LIST, sizeof(*list), 1, TRUE);

    /* create a mutex on multithreaded environments */

    if (list != NULL)
    {
        list->type = type;

         if (OCI_LIB_THREADED)
         {
             list->mutex = OCI_MutexCreateInternal();

            if (list->mutex == NULL)
                OCI_FREE(list);
         }
    }

    return list;
}

/* ------------------------------------------------------------------------ *
 * OCI_ListFree
 * ------------------------------------------------------------------------ */

boolean OCI_ListFree(OCI_List *list)
{
    boolean res = TRUE;

    OCI_CHECK(list == NULL, FALSE);

    OCI_ListClear(list);

    if (list->mutex != NULL)
        res = OCI_MutexFree(list->mutex);

    OCI_FREE(list);
    
    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ListCreateItem
 * ------------------------------------------------------------------------ */

OCI_Item * OCI_ListCreateItem(int type, int size)
{
    OCI_Item *item  = NULL;

    /* allocate list item entry */

    item = (OCI_Item *) OCI_MemAlloc(OCI_IPC_LIST_ITEM, sizeof(*item), 1, TRUE);

    if (item != NULL)
    {
        /* allocate item data buffer */

        item->data = (void *) OCI_MemAlloc(type, size, 1, TRUE);

        if (item->data == NULL)
            OCI_FREE(item);
    }

    return item;
}

/* ------------------------------------------------------------------------ *
 * OCI_ListAppend
 * ------------------------------------------------------------------------ */

OCI_Item * OCI_ListAppend(OCI_List *list, int size)
{
    OCI_Item *item = NULL;
    OCI_Item *temp = NULL;

    OCI_CHECK(list == NULL, NULL);

    item = OCI_ListCreateItem(list->type, size);

    OCI_CHECK(item == NULL, FALSE);

    if (list->mutex != NULL)
        OCI_MutexAcquire(list->mutex);

    temp = list->head;

    while (temp != NULL && temp->next)
    {
        temp = temp->next;
    }

    if (temp != NULL)
        temp->next = item;
    else
        list->head = item;

    list->count++;

    if (list->mutex != NULL)
        OCI_MutexRelease(list->mutex);

    return item;
}

/* ------------------------------------------------------------------------ *
 * OCI_ListClear
 * ------------------------------------------------------------------------ */

boolean OCI_ListClear(OCI_List *list)
{
    OCI_Item *item = NULL;
    OCI_Item *temp = NULL;

    OCI_CHECK(list == NULL, FALSE);

    if (list->mutex != NULL)
        OCI_MutexAcquire(list->mutex);

    /* walk along the list to free item's buffer */

    item = list->head;

    while (item != NULL)
    {
        temp  = item;
        item  = item->next;

        /* free data */

        OCI_FREE(temp->data);
        OCI_FREE(temp);
    }

    list->head  = NULL;
    list->count = 0;

    if (list->mutex != NULL)
        OCI_MutexRelease(list->mutex);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ListForEach
 * ------------------------------------------------------------------------ */

boolean OCI_ListForEach(OCI_List *list, boolean (*proc)(void *))
{
    OCI_Item *item = NULL;

    OCI_CHECK(list == NULL, FALSE);

    if (list->mutex != NULL)
        OCI_MutexAcquire(list->mutex);

    item = list->head;

    /* for each item in the list, execute the given callback */

    while (item != NULL)
    {
        proc(item->data);
        item = item->next;
    }

    if (list->mutex != NULL)
        OCI_MutexRelease(list->mutex);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ListRemove
 * ------------------------------------------------------------------------ */

boolean OCI_ListRemove(OCI_List *list, void *data)
{
    OCI_Item *item = NULL;
    OCI_Item *temp = NULL;

    OCI_CHECK(list == NULL, FALSE);
    OCI_CHECK(data == NULL, FALSE);

    if (list->mutex != NULL)
        OCI_MutexAcquire(list->mutex);

    item = list->head;

    while (item != NULL)
    {
        if (item->data == data)
        {
            if (temp) temp->next = item->next;

            /* if item was the first entry, readjust the first list
               entry to next element */

            if (item == list->head) list->head = item->next;

            OCI_FREE(item);

            break;
        }

        temp = item;
        item = item->next;
    }

    list->count--;

    if (list->mutex != NULL)
        OCI_MutexRelease(list->mutex);

    return TRUE;
}
