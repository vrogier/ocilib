/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2014 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: hash.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashCompute
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_HashCompute
(
    OCI_HashTable *table,
    const otext   *str
)
{
    unsigned int h;
    otext *p;
    otext c;

    OCI_CHECK(table == NULL, 0);
    OCI_CHECK(str   == NULL, 0);

    for(h = 0, p = (otext *) str; (*p) != 0; p++)
    {
        c = *p;

        h = 31 * h + otoupper(c);
    }

    return (h % table->size);
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashCreate
 * --------------------------------------------------------------------------------------------- */

OCI_HashTable * OCI_API OCI_HashCreate
(
    unsigned int size,
    unsigned int type
)
{
    OCI_HashTable *table = NULL;
    boolean res          = FALSE;

    /* allocate table structure */

    table = (OCI_HashTable *) OCI_MemAlloc(OCI_IPC_HASHTABLE, sizeof(*table), (size_t) 1, TRUE);

    /* set up attributes and allocate internal array of hash entry pointers */

    if (table)
    {
        table->type  = type;
        table->size  = 0;
        table->count = 0;

        table->items = (OCI_HashEntry **) OCI_MemAlloc(OCI_IPC_HASHENTRY_ARRAY,
                                                       sizeof(*table->items),
                                                       (size_t) size, TRUE);
        if (table->items)
        {
            table->size = size;           
            
            res = TRUE;
        }
    }

    if (!res)
    {
        OCI_HashFree(table);
    }

    OCI_RESULT(res);

    return table;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_HashFree
(
    OCI_HashTable *table
)
{
    unsigned int i;

    OCI_HashEntry *e1 = NULL, *e2 = NULL;
    OCI_HashValue *v1 = NULL, *v2 = NULL;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, FALSE);

    if (table->items)
    {
        for (i = 0; i < table->size; i++)
        {
            e1 = table->items[i];

            while (e1)
            {
                e2 = e1;
                e1 = e1->next;

                v1 = e2->values;

                while (v1)
                {
                    v2 = v1;
                    v1 = v1->next;

                    if (OCI_HASH_STRING == table->type)
                    {
                        OCI_FREE(v2->value.p_text);
                    }

                    OCI_FREE(v2);
                }

                if (e2->key)
                {
                    OCI_FREE(e2->key);
                }

                if (e2)
                {
                    OCI_FREE(e2);
                }
            }
        }

        OCI_FREE(table->items);
    }

    OCI_FREE(table);

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_HashGetSize
(
    OCI_HashTable *table
)
{
    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, 0);

    OCI_RESULT(TRUE);

    return table->size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_HashGetType
(
    OCI_HashTable *table
)
{
    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return table->type;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashGetValue
 * --------------------------------------------------------------------------------------------- */

OCI_HashValue * OCI_API OCI_HashGetValue
(
    OCI_HashTable *table,
    const otext   *key
)
{
    OCI_HashEntry *e = NULL;
    OCI_HashValue *v = NULL;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, NULL);

    e = OCI_HashLookup(table, key, FALSE);

    if (e)
    {
        v = e->values;
    }

    OCI_RESULT(v != NULL);

    return v;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashGetEntry
 * --------------------------------------------------------------------------------------------- */

OCI_HashEntry * OCI_API OCI_HashGetEntry
(
    OCI_HashTable *table,
    unsigned int   index
)
{
    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, NULL);
    OCI_CHECK_BOUND(NULL, index, 1, table->size, NULL);

    OCI_RESULT(TRUE);

    return table->items[index];
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_HashGetString
(
    OCI_HashTable *table,
    const otext   *key
)
{
    OCI_HashValue *v   = NULL;
    const otext *value = NULL;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, NULL);
    OCI_CHECK(table->type != OCI_HASH_STRING, NULL);

    v = OCI_HashGetValue(table, key);

    if (v)
    {
        value = v->value.p_text;
    }

    OCI_RESULT(v != NULL);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashGetInt
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_HashGetInt
(
    OCI_HashTable *table,
    const otext   *key
)
{
    OCI_HashValue *v = NULL;
    int value        = 0;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, 0);
    OCI_CHECK(table->type != OCI_HASH_INTEGER, 0);

    v = OCI_HashGetValue(table, key);

    if (v)
    {
        value = v->value.num;
    }

    OCI_RESULT(v != NULL);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashGetPointer
 * --------------------------------------------------------------------------------------------- */

void * OCI_API OCI_HashGetPointer
(
    OCI_HashTable *table,
    const otext   *key
)
{
    OCI_HashValue *v = NULL;
    void *value      = NULL;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, NULL);
    OCI_CHECK(table->type != OCI_HASH_POINTER, NULL);

    v = OCI_HashGetValue(table, key);

    if (v)
    {
        value = v->value.p_void;
    }

    OCI_RESULT(v != NULL);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashAdd
 * --------------------------------------------------------------------------------------------- */

boolean OCI_HashAdd
(
    OCI_HashTable *table,
    const otext   *key,
    OCI_Variant    value,
    unsigned int   type
)
{
    OCI_HashEntry * e = NULL;
    OCI_HashValue * v = NULL, *v1 = NULL, *v2 = NULL;
    boolean res = FALSE;

    OCI_CHECK(table == NULL, FALSE);
    OCI_CHECK(key   == NULL, FALSE);
    OCI_CHECK(table->type != type, FALSE);

    e = OCI_HashLookup(table, key, TRUE);

    if (e)
    {
        v = (OCI_HashValue *) OCI_MemAlloc(OCI_IPC_HASHVALUE, sizeof(*v), (size_t) 1, TRUE);

        if (v)
        {
            if (OCI_HASH_STRING == table->type && value.p_text)
            {
                v->value.p_text = ostrdup(value.p_text);
            }
            else if (OCI_HASH_INTEGER == table->type)
            {
                v->value.num = value.num;
            }
            else
            {
                v->value.p_void = value.p_void;
            }

            v1 = v2 = e->values;

            while (v1)
            {
                v2 = v1;
                v1 = v1->next;
            }

            if (v2)
            {
                v2->next = v;
            }
            else
            {
                e->values = v;
            }

            res = TRUE;
        }
    }
    
    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashAddString
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_HashAddString
(
    OCI_HashTable *table,
    const otext   *key,
    const otext   *value
)
{
    boolean res = TRUE;
    OCI_Variant v;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, FALSE);

    v.p_text = (otext *) value;

    res = OCI_HashAdd(table, key, v, OCI_HASH_STRING);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashAddInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_HashAddInt
(
    OCI_HashTable *table,
    const otext   *key,
    int            value
)
{
    boolean res = TRUE;
    OCI_Variant v;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, FALSE);

    v.num = value;

    res = OCI_HashAdd(table, key, v, OCI_HASH_INTEGER);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashAddPointer
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_HashAddPointer
(
    OCI_HashTable *table,
    const otext   *key,
    void          *value
)
{
    boolean res = TRUE;
    OCI_Variant v;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, FALSE);

    v.p_void = value;

    res = OCI_HashAdd(table, key, v, OCI_HASH_POINTER);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashLookup
 * --------------------------------------------------------------------------------------------- */

OCI_HashEntry * OCI_API OCI_HashLookup
(
    OCI_HashTable *table,
    const otext   *key,
    boolean        create
)
{
    OCI_HashEntry *e = NULL, *e1 = NULL, *e2 = NULL;
    unsigned int i;

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table, NULL);
    OCI_CHECK_PTR(OCI_IPC_STRING, key, NULL);

    i = OCI_HashCompute(table, key);

    if (i < table->size)
    {
        for(e = table->items[i]; e; e = e->next)
        {
            if (ostrcasecmp(e->key, key) == 0)
            {
                break;
            }
        }

        if (!e && create)
        {
            e = (OCI_HashEntry *) OCI_MemAlloc(OCI_IPC_HASHENTRY, sizeof(*e), (size_t) 1, TRUE);

            if (e)
            {
                e->key = ostrdup(key);

                e1 = e2 = table->items[i];

                while (e1)
                {
                    e2 = e1;
                    e1 = e1->next;
                }

                if (e2)
                {
                    e2->next = e;
                }
                else
                {
                    table->items[i] = e;
                }
            }
        }
    }

    OCI_RESULT(e != NULL);

    return e;
}
