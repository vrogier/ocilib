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

#include "hash.h"

#include "macros.h"
#include "memory.h"
#include "stringutils.h"

static const OCI_UInt32 HashTypeValues[] =
{
    OCI_HASH_STRING,
    OCI_HASH_INTEGER,
    OCI_HASH_POINTER
};

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashCompute
 * --------------------------------------------------------------------------------------------- */

static OCI_UInt32 OcilibHashCompute
(
    OCI_HashTable *table,
    const otext   *str
)
{
    OCI_UInt32 h;

    otext *p;

    if (NULL == table) return (0);
    if (NULL == str) return (0);

    for(h = 0, p = (otext *) str; (*p) != 0; p++)
    {
        h = 31 * h + otoupper(*p);
    }

    return (h % table->size);
}

/* --------------------------------------------------------------------------------------------- *
* OcilibHashAdd
* --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibHashAdd
(
    OCI_HashTable *table,
    const otext   *key,
    OCI_Variant    value,
    OCI_HashType   type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    OCI_HashEntry * e = NULL;
    OCI_HashValue * v = NULL, *v1 = NULL, *v2 = NULL;

    CHECK_PTR(OCI_IPC_HASHTABLE, table);
    CHECK_PTR(OCI_IPC_STRING,      key);
    CHECK_COMPAT(table->type == type);

    OCI_Boolean success = FALSE;

    e = OcilibHashLookup(table, key, TRUE);

    if (NULL != e)
    {
        v = (OCI_HashValue *)OcilibMemoryAlloc(table->env, OCI_IPC_HASHVALUE, sizeof(*v),
                                               (OCI_Size)1, TRUE);

        if (NULL != v)
        {
            if (OCI_HASH_STRING == table->type && value.p_text)
            {
                v->value.p_text = OcilibStringDuplicate(table->env, value.p_text);
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

            while (NULL != v1)
            {
                v2 = v1;
                v1 = v1->next;
            }

            if (NULL != v2)
            {
                v2->next = v;
            }
            else
            {
                e->values = v;
            }

            success = TRUE;
        }
    }

    SET_RETVAL(success);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashCreate
 * --------------------------------------------------------------------------------------------- */

OCI_HashTable * OcilibHashCreate
(
    OCI_Environment *env,
    OCI_UInt32     size,
    OCI_HashType     type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_HashTable*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_HashTable* table = NULL;

    CHECK_ENUM_VALUE(type, HashTypeValues, OTEXT("Hash type"));

    /* allocate table structure */

    table = (OCI_HashTable *)OcilibMemoryAlloc(env, OCI_IPC_HASHTABLE,
                                               sizeof(*table),
                                               (OCI_Size) 1, TRUE);
    CHECK_NULL(table);

    /* set up attributes and allocate internal array of hash entry pointers */

    table->env   = env;
    table->htype = OCI_IPC_HASHTABLE;
    table->type  = type;
    table->size  = 0;
    table->count = 0;

    table->items = (OCI_HashEntry **)OcilibMemoryAlloc(env, OCI_IPC_HASHENTRY_ARRAY,
                                                       sizeof(*table->items),
                                                       (OCI_Size) size, TRUE);
    CHECK_NULL(table->items);

    table->size = size;

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibHashFree(table);
            table = NULL;
        }

        SET_RETVAL(table);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibHashFree
(
    OCI_HashTable *table
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    OCI_HashEntry *e1 = NULL, *e2 = NULL;
    OCI_HashValue *v1 = NULL, *v2 = NULL;

    CHECK_PTR(OCI_IPC_HASHTABLE, table);

    if (NULL != table->items)
    {
        for (OCI_UInt32 i = 0; i < table->size; i++)
        {
            e1 = table->items[i];

            while (NULL != e1)
            {
                e2 = e1;
                e1 = e1->next;

                v1 = e2->values;

                while (NULL != v1)
                {
                    v2 = v1;
                    v1 = v1->next;

                    if (OCI_HASH_STRING == table->type)
                    {
                        FREE(v2->value.p_text);
                    }

                    FREE(v2);
                }

                if (NULL != e2->key)
                {
                    FREE(e2->key);
                }

                FREE(e2);
            }
        }

        FREE(table->items);
    }

    OcilibErrorResetSource(NULL, table);

    FREE(table);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * HashGetSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibHashGetSize
(
    OCI_HashTable *table
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    CHECK_PTR(OCI_IPC_HASHTABLE, table);

    SET_RETVAL(table->size);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashGetType
 * --------------------------------------------------------------------------------------------- */

OCI_HashType OcilibHashGetType
(
    OCI_HashTable *table
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_HashType, OCI_UNKNOWN,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    CHECK_PTR(OCI_IPC_HASHTABLE, table);

    SET_RETVAL(table->type);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashGetValue
 * --------------------------------------------------------------------------------------------- */

OCI_HashValue * OcilibHashGetValue
(
    OCI_HashTable *table,
    const otext   *key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_HashValue*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    CHECK_PTR(OCI_IPC_HASHTABLE, table);

    OCI_HashEntry* e = OcilibHashLookup(table, key, FALSE);
    CHECK_NULL(e);

    SET_RETVAL(e->values);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashGetEntry
 * --------------------------------------------------------------------------------------------- */

OCI_HashEntry * OcilibHashGetEntry
(
    OCI_HashTable *table,
    OCI_UInt32   index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_HashEntry*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    CHECK_PTR(OCI_IPC_HASHTABLE, table);

    CHECK(index < table->size);

    SET_RETVAL(table->items[index]);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibHashGetString
(
    OCI_HashTable *table,
    const otext   *key
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    CHECK_PTR(OCI_IPC_HASHTABLE, table);
    CHECK_COMPAT(table->type == OCI_HASH_STRING);

    OCI_HashValue *v = OcilibHashGetValue(table, key);
    CHECK_NULL(v);

    SET_RETVAL(v->value.p_text);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashGetInt
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibHashGetInt
(
    OCI_HashTable *table,
    const otext   *key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, 0,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    CHECK_PTR(OCI_IPC_HASHTABLE, table);
    CHECK_COMPAT(table->type == OCI_HASH_INTEGER);

    OCI_HashValue * v = OcilibHashGetValue(table, key);
    CHECK_NULL(v);

    SET_RETVAL(v->value.num);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashGetPointer
 * --------------------------------------------------------------------------------------------- */

void * OcilibHashGetPointer
(
    OCI_HashTable *table,
    const otext   *key
)
{
    ENTER_FUNC
    (
        /* returns */ void*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    CHECK_PTR(OCI_IPC_HASHTABLE, table);
    CHECK_COMPAT(table->type == OCI_HASH_POINTER);

    OCI_HashValue *v = OcilibHashGetValue(table, key);
    CHECK_NULL(v);

    SET_RETVAL(v->value.p_void);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashAddString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibHashAddString
(
    OCI_HashTable *table,
    const otext   *key,
    const otext   *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    OCI_Variant v;

    CHECK_PTR(OCI_IPC_HASHTABLE, table);
    CHECK_COMPAT(table->type == OCI_HASH_STRING);

    v.p_text = (otext *) value;

    CHECK(OcilibHashAdd(table, key, v, OCI_HASH_STRING));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashAddInt
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibHashAddInt
(
    OCI_HashTable *table,
    const otext   *key,
    OCI_Int32            value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    OCI_Variant v;

    CHECK_PTR(OCI_IPC_HASHTABLE, table);
    CHECK_COMPAT(table->type == OCI_HASH_INTEGER);

    v.num = value;

    CHECK(OcilibHashAdd(table, key, v, OCI_HASH_INTEGER));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashAddPointer
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibHashAddPointer
(
    OCI_HashTable *table,
    const otext   *key,
    void          *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    OCI_Variant v;

    CHECK_PTR(OCI_IPC_HASHTABLE, table);
    CHECK_COMPAT(table->type == OCI_HASH_POINTER);

    v.p_void = value;

    CHECK(OcilibHashAdd(table, key, v, OCI_HASH_POINTER));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHashLookup
 * --------------------------------------------------------------------------------------------- */

OCI_HashEntry * OcilibHashLookup
(
    OCI_HashTable *table,
    const otext   *key,
    OCI_Boolean    create
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_HashEntry*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, (table ? table->env : NULL)
    );

    OCI_HashEntry *e = NULL, *e1 = NULL, *e2 = NULL;

    CHECK_PTR(OCI_IPC_HASHTABLE, table);
    CHECK_PTR(OCI_IPC_STRING,      key);

    const OCI_UInt32 i = OcilibHashCompute(table, key);

    if (i < table->size)
    {
        for(e = table->items[i]; e; e = e->next)
        {
            if (OcilibStringCaseCompare(e->key, key) == 0)
            {
                break;
            }
        }

        if (NULL == e && create)
        {
            e = (OCI_HashEntry *)OcilibMemoryAlloc(table->env, OCI_IPC_HASHENTRY,
                                                   sizeof(*e),
                                                   (OCI_Size) 1,
                                                   TRUE);
            CHECK_NULL(e);

            e->key = OcilibStringDuplicate(table->env, key);
            CHECK_NULL(e->key);

            e1 = e2 = table->items[i];

            while (NULL != e1)
            {
                e2 = e1;
                e1 = e1->next;
            }

            if (NULL != e2)
            {
                e2->next = e;
            }
            else
            {
                table->items[i] = e;
            }
        }
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && create)
        {
            FREE(e);
        }

        SET_RETVAL(e);
    );
}
