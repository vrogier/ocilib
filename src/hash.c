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
 *                             PRIVATE VARIABLES
 * ********************************************************************************************* */

static unsigned int HashTypeValues[] = { OCI_HASH_STRING, OCI_HASH_INTEGER, OCI_HASH_POINTER };


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

    OCI_CHECK(NULL == table, 0);
    OCI_CHECK(NULL == str, 0);

    for(h = 0, p = (otext *) str; (*p) != 0; p++)
    {
        otext c = *p;

        h = 31 * h + otoupper(c);
    }

    return (h % table->size);
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

    OCI_CHECK(NULL == table, FALSE)
    OCI_CHECK(NULL == key, FALSE)
    OCI_CHECK(table->type != type, FALSE)

    e = OCI_HashLookup(table, key, TRUE);

    if (e)
    {
        v = (OCI_HashValue *)OCI_MemAlloc(OCI_IPC_HASHVALUE, sizeof(*v), (size_t)1, TRUE);

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

    return res;
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

    OCI_LIB_CALL_ENTER(OCI_HashTable*, table)

    OCI_CHECK_ENUM_VALUE(NULL, NULL, type, HashTypeValues, OTEXT("Hash type"));

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
            
            call_status = TRUE;
        }
    }

    if (call_status)
    {
        call_retval = table;
    }
    else if (table)
    {
        OCI_HashFree(table);
    }

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table)

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
                        OCI_FREE(v2->value.p_text)
                    }

                    OCI_FREE(v2)
                }

                if (e2->key)
                {
                    OCI_FREE(e2->key)
                }

                if (e2)
                {
                    OCI_FREE(e2)
                }
            }
        }

        OCI_FREE(table->items)
    }

    OCI_FREE(table)

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_HashGetSize
(
    OCI_HashTable *table
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table)

    call_retval = table->size;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_HashGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_HashGetType
(
    OCI_HashTable *table
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table)

    call_retval = table->type;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()}

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

    OCI_LIB_CALL_ENTER(OCI_HashValue*, NULL)

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table)

    e = OCI_HashLookup(table, key, FALSE);

    if (e)
    {
        call_retval = e->values;
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(OCI_HashEntry*, NULL)

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table)
    
    if (index < table->size)
    {
        call_retval = table->items[index];
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    OCI_HashValue *v = NULL;

    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table)
    OCI_CHECK_COMPAT(NULL, table->type == OCI_HASH_STRING)

    v = OCI_HashGetValue(table, key);

    if (v)
    {
        call_retval = v->value.p_text;     
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(int, 0)

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table)
    OCI_CHECK_COMPAT(NULL, table->type == OCI_HASH_INTEGER)

    v = OCI_HashGetValue(table, key);

    if (v)
    {
        call_retval = v->value.num;
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(void *, NULL)

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table)
    OCI_CHECK_COMPAT(NULL, table->type == OCI_HASH_POINTER)

    v = OCI_HashGetValue(table, key);

    if (v)
    {
        call_retval = v->value.p_void;
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    OCI_Variant v;

    OCI_LIB_CALL_ENTER(boolean, FALSE)
        
    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table)
    OCI_CHECK_COMPAT(NULL, table->type == OCI_HASH_STRING)

    v.p_text = (otext *) value;

    call_retval = call_status = OCI_HashAdd(table, key, v, OCI_HASH_STRING);

    OCI_LIB_CALL_EXIT()
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
    OCI_Variant v;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table)
    OCI_CHECK_COMPAT(NULL, table->type == OCI_HASH_INTEGER)

    v.num = value;

    call_retval = call_status = OCI_HashAdd(table, key, v, OCI_HASH_INTEGER);

    OCI_LIB_CALL_EXIT()
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
    OCI_Variant v;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table)
    OCI_CHECK_COMPAT(NULL, table->type == OCI_HASH_POINTER)

    v.p_void = value;

    call_retval = call_status = OCI_HashAdd(table, key, v, OCI_HASH_POINTER);

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(OCI_HashEntry*, NULL)

    OCI_CHECK_PTR(OCI_IPC_HASHTABLE, table)
    OCI_CHECK_PTR(OCI_IPC_STRING, key)

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

    call_retval = e;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}
