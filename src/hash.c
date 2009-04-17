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
 * $Id: hash.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_HashCompute
 * ------------------------------------------------------------------------ */

unsigned int OCI_HashCompute(OCI_HashTable *table, const mtext *str)
{
    unsigned int h;
    mtext *p;
    mtext c;

    OCI_CHECK(table == NULL, 0);
    OCI_CHECK(str   == NULL, 0);

    for(h = 0, p = (mtext *) str; (*p) != 0; p++)
    {
        c = *p;

        h = 31 * h + mttoupper(c);
    }

    return (h % table->size);
}

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_HashCreate
 * ------------------------------------------------------------------------ */

OCI_HashTable * OCI_API OCI_HashCreate(unsigned int size, unsigned int type)
{
    OCI_HashTable *table = NULL;
    boolean res = TRUE;
    
    /* allocate table structure */

    table = (OCI_HashTable *) OCI_MemAlloc(OCI_IPC_HASHTABLE, sizeof(*table), 1, TRUE);

    /* set up attributes and allocate internal array of hash entry pointers */

    if (table != NULL)
    {
        table->size     = size;
        table->type     = type;
        table->count    = 0;

        table->items = (OCI_HashEntry **) OCI_MemAlloc(OCI_IPC_HASHENTRY_ARRAY,
                                                       sizeof(*table->items),
                                                       size, TRUE);
        res = (table->items != NULL);
    }
    else
        res = FALSE;

    if (res == FALSE)
        OCI_HashFree(table);

    OCI_RESULT(res);

    return table;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_HashFree(OCI_HashTable *table)
{
    unsigned int i;

    OCI_HashEntry *e1, *e2;
    OCI_HashValue *v1, *v2;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, FALSE);

    for (i = 0; i < table->size; i++)
    {
        e1 = table->items[i];

        while (e1 != NULL)
        {
            e2 = e1;
            e1 = e1->next;

            v1 = e2->values;

            while (v1 != NULL)
            {
                v2 = v1;
                v1 = v1->next;

                if (table->type == OCI_HASH_STRING)
                    OCI_FREE(v2->value.p_mtext);

                OCI_FREE(v2);
            }

            if (e2->key)
                OCI_FREE(e2->key);

            if (e2)
                OCI_FREE(e2);
        }
    }

    if (table->items != NULL)
    {
        OCI_FREE(table->items);
    }

    OCI_FREE(table);

    OCI_RESULT(TRUE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetSize
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_HashGetSize(OCI_HashTable *table)
{
    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, 0);

    OCI_RESULT(TRUE);

    return table->size;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetType
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_HashGetType(OCI_HashTable *table)
{
    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return table->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetValue
 * ------------------------------------------------------------------------ */

OCI_HashValue * OCI_API OCI_HashGetValue(OCI_HashTable *table, const mtext *key)
{
    OCI_HashEntry *e = NULL;
    OCI_HashValue *v = NULL;
 
    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, NULL);

    e = OCI_HashLookup(table, key, FALSE);

    if (e != NULL)
        v = e->values;

    OCI_RESULT(v != NULL);

    return v;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetEntry
 * ------------------------------------------------------------------------ */

OCI_HashEntry * OCI_API OCI_HashGetEntry(OCI_HashTable *table, unsigned int index)
{
    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, NULL);
    OCI_CHECK_BOUND(NULL, index, 1, table->size, NULL);

    OCI_RESULT(TRUE);

    return table->items[index];
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetString
 * ------------------------------------------------------------------------ */

const mtext * OCI_API OCI_HashGetString(OCI_HashTable *table, const mtext *key)
{
    OCI_HashValue *v   = NULL;
    const mtext *value = NULL;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, NULL);
    OCI_CHECK(table->type != OCI_HASH_STRING, NULL);

    v = OCI_HashGetValue(table, key);

    if (v != NULL)
    {
        value = v->value.p_mtext;
    }

    OCI_RESULT(v != NULL);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetInt
 * ------------------------------------------------------------------------ */

int OCI_API OCI_HashGetInt(OCI_HashTable *table, const mtext *key)
{
    OCI_HashValue *v = NULL;
    int value = 0;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, 0);
    OCI_CHECK(table->type != OCI_HASH_INTEGER, 0);

    v = OCI_HashGetValue(table, key);

    if (v != NULL)
    {
        value = v->value.num;
    }

    OCI_RESULT(v != NULL);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashGetPointer
 * ------------------------------------------------------------------------ */

void * OCI_API OCI_HashGetPointer(OCI_HashTable *table, const mtext *key)
{
    OCI_HashValue *v = NULL;
    void *value = NULL;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, NULL);
    OCI_CHECK(table->type != OCI_HASH_POINTER, NULL);

    v = OCI_HashGetValue(table, key);

    if (v != NULL)
    {
        value = v->value.p_void;
    }

    OCI_RESULT(v != NULL);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashAdd
 * ------------------------------------------------------------------------ */

boolean OCI_HashAdd(OCI_HashTable *table, const mtext *key, OCI_Variant value, 
                    unsigned int type)
{
    OCI_HashEntry * e = NULL;
    OCI_HashValue * v = NULL, *v1 = NULL, *v2 = NULL;

    OCI_CHECK(table == NULL, FALSE);
    OCI_CHECK(key   == NULL, FALSE);
    OCI_CHECK(table->type != type, FALSE);

    e = OCI_HashLookup(table, key, TRUE);

    if (e != NULL)
    {
        v = (OCI_HashValue *) OCI_MemAlloc(OCI_IPC_HASHVALUE, sizeof(*v), 1, TRUE);

        if (v != NULL)
        {
            if (table->type == OCI_HASH_STRING && value.p_mtext != NULL)
            {
                v->value.p_mtext = mtsdup(value.p_mtext);
            }
            else if (table->type == OCI_HASH_INTEGER)
            {
                v->value.num = value.num;
            }
            else
                v->value.p_void = value.p_void;

            v1 = v2 = e->values;

            while (v1 != NULL)
            {
                v2 = v1;
                v1 = v1->next;
            }

            if (v2 != NULL)
                v2->next  = v;
            else
                e->values = v;
        }
    }

    return (v != NULL);
}

/* ------------------------------------------------------------------------ *
 * OCI_HashAddString
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_HashAddString(OCI_HashTable *table, const mtext *key, 
                                  const mtext *value)
{
    boolean res = TRUE;
    OCI_Variant v;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, FALSE);

    v.p_mtext = (mtext *) value;

    res = OCI_HashAdd(table, key, v, OCI_HASH_STRING);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashAddInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_HashAddInt(OCI_HashTable *table, const mtext *key,
                               int value)
{
    boolean res = TRUE;
    OCI_Variant v;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, FALSE);

    v.num = value;

    res = OCI_HashAdd(table, key, v, OCI_HASH_INTEGER);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashAddPointer
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_HashAddPointer(OCI_HashTable *table, const mtext *key, 
                                   void *value)
{
    boolean res = TRUE;
    OCI_Variant v;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, FALSE);

    v.p_void = value;

    res  = OCI_HashAdd(table, key, v, OCI_HASH_POINTER);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_HashLookup
 * ------------------------------------------------------------------------ */

OCI_HashEntry * OCI_API OCI_HashLookup(OCI_HashTable *table, const mtext *key,
                                       boolean create)
{
    OCI_HashEntry *e = NULL, *e1 = NULL, *e2 = NULL;
    unsigned int i;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, NULL);
    OCI_CHECK_PTR(OCI_IPC_STRING, key, NULL);

    i = OCI_HashCompute(table, key);

    if (i < table->size)
    {
        for(e = table->items[i]; e != NULL; e = e->next)
        {
            if (mtscasecmp(e->key, key) == 0)
                break;
        }

        if ((e == NULL) && (create == TRUE))
        {
            e = (OCI_HashEntry *) OCI_MemAlloc(OCI_IPC_HASHENTRY, sizeof(*e), 1, TRUE);

            if (e != NULL)
            {
                e->key = mtsdup(key);

                e1 = e2 = table->items[i];

                while (e1 != NULL)
                {
                    e2 = e1;
                    e1 = e1->next;
                }

                if (e2 != NULL)
                    e2->next = e;
                else
                    table->items[i] = e;
            }
        }
    }

    OCI_RESULT(e != NULL);

    return e;
}
