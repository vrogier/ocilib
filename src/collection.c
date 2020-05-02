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

#include "collection.h"

#include "array.h"
#include "element.h"
#include "macros.h"
#include "strings.h"

/* --------------------------------------------------------------------------------------------- *
 * CollInit
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * CollectionInitialize
(
    OCI_Connection *con,
    OCI_Coll       *coll,
    void           *handle,
    OCI_TypeInfo   *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Coll*, coll,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    ALLOC_DATA(OCI_IPC_COLLECTION, coll, 1);

    coll->con    = con;
    coll->handle = handle;
    coll->typinf = typinf;

    if (NULL == coll->handle || (OCI_OBJECT_ALLOCATED_ARRAY == coll->hstate))
    {
        /* allocates handle for non fetched collection */

        if (OCI_OBJECT_ALLOCATED_ARRAY != coll->hstate)
        {
            coll->hstate = OCI_OBJECT_ALLOCATED;
        }

        CHECK
        (
            MemoryAllocateObject
            (
                coll->con->env, coll->con->err, coll->con->cxt,
                typinf->colcode, typinf->tdo, (void *) NULL,
                OCI_DURATION_SESSION, TRUE, (dvoid **) &coll->handle
            )
        )
    }
    else
    {
        coll->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

    CLEANUP_AND_EXIT_FUNC
    (       
        if (FAILURE)
        {
            CollectionFree(coll);
            coll = NULL;
        }

        SET_RETVAL(coll)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * CollCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * CollectionCreate
(
    OCI_TypeInfo *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Coll*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    )

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)

    SET_RETVAL(CollectionInitialize(typinf->con, NULL, NULL, typinf))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollFree
 * --------------------------------------------------------------------------------------------- */

boolean CollectionFree
(
    OCI_Coll *coll
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CHECK_OBJECT_FETCHED(coll)

    /* free data element */

    if (coll->elem)
    {
        coll->elem->hstate = OCI_OBJECT_FETCHED_DIRTY;

        ElementFree(coll->elem);

        coll->elem = NULL;
    }

    /* free collection for local object */

    if ((OCI_OBJECT_ALLOCATED == coll->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == coll->hstate))
    {
        MemoryFreeObject(coll->con->env, coll->typinf->con->err, coll->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != coll->hstate)
    {
        FREE(coll)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Coll ** CollectionCreateArray
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Coll**, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    arr = ArrayCreate(con, nbelem, OCI_CDT_COLLECTION, 0,
              sizeof(OCIColl*), sizeof(OCI_Coll), 0, typinf);

    CHECK_NULL(arr)

    SET_RETVAL((OCI_Coll **) arr->tab_obj)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean CollectionFreeArray
(
    OCI_Coll **colls
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, colls
    )

    CHECK_PTR(OCI_IPC_ARRAY, colls)

    SET_RETVAL(ArrayFreeFromHandles((void **)colls))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollAssign
 * --------------------------------------------------------------------------------------------- */

boolean CollectionAssign
(
    OCI_Coll *coll,
    OCI_Coll *coll_src
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CHECK_PTR(OCI_IPC_COLLECTION, coll_src)
    CHECK_COMPAT(coll->typinf->cols[0].libcode == coll_src->typinf->cols[0].libcode)

    CHECK_OCI
    (
        coll->con->err, 
        OCICollAssign,
        coll->con->env, coll->con->err, 
        coll_src->handle, coll->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int CollectionGetType
(
    OCI_Coll *coll
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, OCI_UNKNOWN,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    CHECK_PTR(OCI_IPC_COLLECTION, coll)

    unsigned int type = OCI_UNKNOWN;

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        type = OCI_COLL_NESTED_TABLE;
    }
    else if (OCI_TYPECODE_VARRAY == coll->typinf->colcode)
    {
        type = OCI_COLL_VARRAY;
    }

#if OCI_VERSION_COMPILE >= OCI_12_1

    else if (OCI_TYPECODE_ITABLE == coll->typinf->colcode)
    {
        type = OCI_COLL_INDEXED_TABLE;
    }

#endif

    SET_RETVAL(type)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetMax
 * --------------------------------------------------------------------------------------------- */

unsigned int CollectionGetMax
(
    OCI_Coll *coll
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    CHECK_PTR(OCI_IPC_COLLECTION, coll)

    SET_RETVAL((unsigned int) OCICollMax(coll->con->env, coll->handle))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int CollectionGetSize
(
    OCI_Coll *coll
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    sb4 value = 0;

    CHECK_PTR(OCI_IPC_COLLECTION, coll)

    CHECK_OCI
    (
        coll->con->err, 
        OCICollSize, 
        coll->con->env, coll->con->err,
        coll->handle, &value
    )

    SET_RETVAL((unsigned int)value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollTrim
 * --------------------------------------------------------------------------------------------- */

boolean CollectionTrim
(
    OCI_Coll    *coll,
    unsigned int nb_elem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CHECK_BOUND((sb4) nb_elem, (sb4) 0, (sb4) CollectionGetSize(coll));

    CHECK_OCI
    (
        coll->con->err,
        OCICollTrim, 
        coll->con->env, coll->con->err, 
        (sb4) nb_elem, coll->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetElem
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * CollectionGetElement
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Elem*, NULL,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    CHECK_PTR(OCI_IPC_COLLECTION, coll)

    boolean exists = FALSE;
    void* data = NULL;
    OCIInd* ind = NULL;

    CHECK_OCI
    (
        coll->con->err,
        OCICollGetElem,
        coll->con->env, coll->con->err, coll->handle,
        (sb4)index - 1, &exists, &data,
        (dvoid**)(dvoid*)&ind
    )

    OCI_Elem* elem = NULL;

    if (exists && NULL != data)
    {
        elem = coll->elem = ElementInitialize(coll->con, coll->elem, data, ind, coll->typinf);
        CHECK_NULL(elem)
    }

    SET_RETVAL(elem)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetElem2
 * --------------------------------------------------------------------------------------------- */

boolean CollectionGetElement2
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    boolean exists = FALSE;
    void   *data   = NULL;
    OCIInd *ind    = NULL;

    CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CHECK_COMPAT(elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype)

    CHECK_OCI
    (
        coll->con->err,
        OCICollGetElem, 
        coll->con->env, coll->con->err, 
        coll->handle, (sb4) index-1, &exists,
        &data, (dvoid **) (dvoid *) &ind
    )

    if (exists && NULL != data)
    {
        elem = ElementInitialize(coll->con, elem, data, ind, coll->typinf);
        CHECK_NULL(elem)
    }
    else
    {
        CHECK(ElementSetNullIndicator(elem, OCI_IND_NULL))
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollSetElem
 * --------------------------------------------------------------------------------------------- */

boolean CollectionSetElement
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CHECK_COMPAT(elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype)

    CHECK_OCI
    (
        coll->con->err,
        OCICollAssignElem,
        coll->con->env, coll->con->err, 
        (sb4) index-1, elem->handle,
        elem->pind, coll->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollAppend
 * --------------------------------------------------------------------------------------------- */

boolean CollectionAddElement
(
    OCI_Coll *coll,
    OCI_Elem *elem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CHECK_COMPAT(elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype)

    CHECK_OCI
    (
        coll->con->err,
        OCICollAppend,
        coll->con->env, coll->con->err, 
        elem->handle, elem->pind, coll->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * CollectionGetTypeInfo
(
    OCI_Coll *coll
)
{
    GET_PROP
    (
        /* result */ OCI_TypeInfo*, NULL,
        /* handle */ OCI_IPC_COLLECTION, coll,
        /* member */ typinf
    )
}

/* --------------------------------------------------------------------------------------------- *
 * CollClear
 * --------------------------------------------------------------------------------------------- */

boolean CollectionClear
(
    OCI_Coll *coll
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    CHECK_PTR(OCI_IPC_COLLECTION, coll)

    CHECK(CollectionTrim(coll, CollectionGetSize(coll)))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollDeleteElem
 * --------------------------------------------------------------------------------------------- */

boolean CollectionRemoveElement
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    CHECK_PTR(OCI_IPC_COLLECTION, coll)

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        CHECK_OCI
        (
            coll->con->err,
            OCITableDelete, 
            coll->con->env, coll->con->err, 
            (sb4) index-1, coll->handle
        )
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetCount
 * --------------------------------------------------------------------------------------------- */

unsigned int CollectionGetCount
(
    OCI_Coll *coll
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    )

     sb4 value = 0;

    CHECK_PTR(OCI_IPC_COLLECTION, coll)

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        CHECK_OCI
        (
            coll->con->err,
            OCITableSize, 
            coll->con->env, coll->con->err,
            coll->handle, &value
        )
    }
    else
    {
        CHECK_OCI
        (
            coll->con->err,
            OCICollSize, 
            coll->con->env, coll->con->err,
            coll->handle, &value
        )
    }

    SET_RETVAL((unsigned int) value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * CollectionToString
 * --------------------------------------------------------------------------------------------- */

boolean CollectionToString
(
    OCI_Coll     *coll,
    unsigned int *size,
    otext        *str
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    )

    OCI_Error   *err   = NULL;
    boolean      quote = TRUE;

    unsigned int len   = 0;
    unsigned int count = 0;

    CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CHECK_PTR(OCI_IPC_VOID, size)

    err = ErrorGet(TRUE, TRUE);

    if (NULL != str)
    {
        *str = 0;
    }

    len += StringAddToBuffer(str, len, coll->typinf->name, 
                       (unsigned int)ostrlen(coll->typinf->name), FALSE);

    len += StringAddToBuffer(str, len, OTEXT("("), 1, FALSE);

    count = CollectionGetSize(coll);

    for (unsigned int i = 1; i <= count; i++)
    {
        OCI_Elem* elem = CollectionGetElement(coll, i);

        quote = TRUE;

        if (ElementIsNull(elem))
        {
            len += StringAddToBuffer(str, len, OCI_STRING_NULL, OCI_STRING_NULL_SIZE, FALSE);
        }
        else
        {
            void              *data      = NULL;
            unsigned int       data_size = 0;
            const unsigned int data_type = coll->typinf->cols[0].datatype;

            otext* tmpbuf = NULL;

            switch (data_type)
            {
                case OCI_CDT_TEXT:
                {
                    data_size = OCIStringSize(Env.env, elem->handle);
                    data = (void*) ElementGetString(elem);
                    break;
                }
                case OCI_CDT_NUMERIC:
                {
                    data  = (void *) elem->handle;
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_BOOLEAN:
                {
                    data  = (void *)elem->handle;
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_RAW:
                {
                    data = elem->handle;
                    if (data)
                    {
                        data_size = OCIRawSize(coll->con->env, (OCIRaw *) data);
                        data      = OCIRawPtr(coll->con->env, (OCIRaw *) data);
                    }
                    break;
                }
                case OCI_CDT_DATETIME:
                {
                    data = (void*) ElementGetDate(elem);
                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    data = (void*) ElementGetTimestamp(elem);
                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    data = (void*) ElementGetInterval(elem);
                    break;
                }
                case OCI_CDT_LOB:
                {
                    data = (void*) ElementGetLob(elem);
                    break;
                }
                case OCI_CDT_FILE:
                {
                    data = (void*) ElementGetFile(elem);
                    break;
                }
                case OCI_CDT_REF:
                {
                    data = (void*) ElementGetReference(elem);
                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    data = (void*) ElementGetObject(elem);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    data = (void*) ElementGetCollection(elem);
                    quote = FALSE;
                }
            }

            CHECK(NULL != data || OCI_CDT_TEXT == data_type)
            CHECK_ERROR(err)

            tmpbuf = str;

            if (NULL != tmpbuf)
            {
                tmpbuf += len;
            }

            if (NULL != data)
            {
                len += StringGetFromType(coll->con, &coll->typinf->cols[0], data, data_size, tmpbuf,
                                         tmpbuf && size ? *size - len : 0, quote);
            }
            else
            {
                len += StringAddToBuffer(str, len, OCI_STRING_NULL,
                    OCI_STRING_NULL_SIZE, FALSE);
            }

            CHECK(NULL == err || OCI_UNKNOWN == err->type)
        }

        if (i < count)
        {
            len += StringAddToBuffer(str, len, OTEXT(", "), 2, FALSE);
        }
    }

    len += StringAddToBuffer(str, len, OTEXT(")"), 1, FALSE);

    *size = len;

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            *size = 0;

            if (NULL != str)
            {
                *str = 0;
            }
        }
    )
}
