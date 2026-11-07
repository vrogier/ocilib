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

#include "collection.h"

#include "array.h"
#include "element.h"
#include "macros.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OcilibCollectionInitialize
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
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    ALLOC_HANDLE(OCI_IPC_COLLECTION, coll);

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
            OcilibMemoryAllocateObject
            (
                coll->con->env, coll->con->env->handle, coll->con->err, coll->con->handle,
                typinf->colcode, typinf->handle, (void *) NULL,
                OCI_DURATION_SESSION, TRUE, (dvoid **) &coll->handle
            )
        );
    }
    else
    {
        coll->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibCollectionFree(coll);
            coll = NULL;
        }

        SET_RETVAL(coll);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OcilibCollectionCreate
(
    OCI_TypeInfo *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Coll*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);

    SET_RETVAL(OcilibCollectionInitialize(typinf->con, NULL, NULL, typinf));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionFree
(
    OCI_Coll *coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);
    CHECK_OBJECT_FETCHED(coll);

    /* free data element */

    if (coll->elem)
    {
        coll->elem->hstate = OCI_OBJECT_FETCHED_DIRTY;

        OcilibElementFree(coll->elem);

        coll->elem = NULL;
    }

    /* free collection for local object */

    if ((OCI_OBJECT_ALLOCATED == coll->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == coll->hstate))
    {
        OcilibMemoryFreeObject(coll->con->env, coll->con->env->handle, coll->con->err, coll->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != coll->hstate)
    {
        OcilibErrorResetSource(NULL, coll);

        FREE(coll);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Coll ** OcilibCollectionCreateArray
(
    OCI_TypeInfo *typinf,
    OCI_UInt32    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Coll**, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);

    arr = OcilibArrayCreate(typinf->con->env, typinf->con, nbelem, OCI_CDT_COLLECTION, 0,
                            sizeof(OCIColl*), sizeof(OCI_Coll), 0, typinf);

    CHECK_NULL(arr);

    SET_RETVAL((OCI_Coll **) arr->tab_obj);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionFreeArray
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionFreeArray
(
    OCI_Coll **colls
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, colls
    );

    CHECK_PTR(OCI_IPC_ARRAY, colls);

    OCI_Coll *col = colls[0];

    CHECK_PTR(OCI_IPC_COLLECTION, col);

    SET_RETVAL(OcilibArrayFreeFromHandles(col->con->env, (void **)colls));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionAssign
(
    OCI_Coll *coll,
    OCI_Coll *coll_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);
    CHECK_PTR(OCI_IPC_COLLECTION, coll_src);
    CHECK_COMPAT(coll->typinf->cols[0].libcode == coll_src->typinf->cols[0].libcode);

    CHECK_OCI
    (
        coll->con->err,
        OCICollAssign,
        coll->con->env->handle, coll->con->err,
        coll_src->handle, coll->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionGetType
 * --------------------------------------------------------------------------------------------- */

OCI_CollType OcilibCollectionGetType
(
    OCI_Coll *coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_CollType, OCI_UNKNOWN,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);

    OCI_CollType type = OCI_UNKNOWN;

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

    SET_RETVAL(type);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionGetMax
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibCollectionGetMax
(
    OCI_Coll *coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);

    OCI_UInt32 coll_max = 0;

    OCI_CALL(coll_max, OCICollMax, coll->con->env->handle, coll->handle);

    SET_RETVAL(coll_max);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionGetSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibCollectionGetSize
(
    OCI_Coll *coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    sb4 value = 0;

    CHECK_PTR(OCI_IPC_COLLECTION, coll);

    CHECK_OCI
    (
        coll->con->err,
        OCICollSize,
        coll->con->env->handle, coll->con->err,
        coll->handle, &value
    );

    SET_RETVAL((OCI_UInt32)value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionTrim
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionTrim
(
    OCI_Coll  *coll,
    OCI_UInt32 nb_elem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);
    CHECK_BOUND((sb4) nb_elem, (sb4) 0, (sb4)OcilibCollectionGetSize(coll));

    CHECK_OCI
    (
        coll->con->err,
        OCICollTrim,
        coll->con->env->handle, coll->con->err,
        (sb4) nb_elem, coll->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionGetElement
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * OcilibCollectionGetElement
(
    OCI_Coll  *coll,
    OCI_UInt32 index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Elem*, NULL,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);

    OCI_Boolean exists = FALSE;
    void      * data   = NULL;
    OCIInd    * ind    = NULL;

    CHECK_OCI
    (
        coll->con->err,
        OCICollGetElem,
        coll->con->env->handle, coll->con->err, coll->handle,
        (sb4)index, (boolean*)  &exists, &data,
        (dvoid**)(dvoid*)&ind
    );

    OCI_Elem* elem = NULL;

    if (exists && NULL != data)
    {
        elem = coll->elem = OcilibElementInitialize(coll->elem, data, ind, coll->typinf);
        CHECK_NULL(elem);
    }

    SET_RETVAL(elem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionSetElement
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionSetElement
(
    OCI_Coll  *coll,
    OCI_UInt32 index,
    OCI_Elem  *elem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);
    CHECK_PTR(OCI_IPC_ELEMENT,    elem);
    CHECK_COMPAT(elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype);

    CHECK_OCI
    (
        coll->con->err,
        OCICollAssignElem,
        coll->con->env->handle, coll->con->err,
        (sb4)index, elem->handle,
        elem->pind, coll->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionAddElement
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionAddElement
(
    OCI_Coll *coll,
    OCI_Elem *elem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);
    CHECK_PTR(OCI_IPC_ELEMENT,    elem);
    CHECK_COMPAT(elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype);

    CHECK_OCI
    (
        coll->con->err,
        OCICollAppend,
        coll->con->env->handle, coll->con->err,
        elem->handle, elem->pind, coll->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OcilibCollectionGetTypeInfo
(
    OCI_Coll *coll
)
{
    GET_PROP
    (
        /* result */ OCI_TypeInfo*, NULL,
        /* handle */ OCI_IPC_COLLECTION, coll,
        /* member */ typinf
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionClear
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionClear
(
    OCI_Coll *coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);

    CHECK(OcilibCollectionTrim(coll, OcilibCollectionGetSize(coll)));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionRemoveElement
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionRemoveElement
(
    OCI_Coll  *coll,
    OCI_UInt32 index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        CHECK_OCI
        (
            coll->con->err,
            OCITableDelete,
            coll->con->env->handle, coll->con->err,
            (sb4)index, coll->handle
        );
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionGetCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibCollectionGetCount
(
    OCI_Coll *coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    sb4 value = 0;

    CHECK_PTR(OCI_IPC_COLLECTION, coll);

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        CHECK_OCI
        (
            coll->con->err,
            OCITableSize,
            coll->con->env->handle, coll->con->err,
            coll->handle, &value
        );
    }
    else
    {
        CHECK_OCI
        (
            coll->con->err,
            OCICollSize,
            coll->con->env->handle, coll->con->err,
            coll->handle, &value
        );
    }

    SET_RETVAL((OCI_UInt32) value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionToString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionToString
(
    OCI_Coll   *coll,
    OCI_UInt32 *size,
    otext      *str
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    OCI_Error  *err   = NULL;
    OCI_Boolean quote = TRUE;

    OCI_UInt32 len   = 0;
    OCI_UInt32 count = 0;

    CHECK_PTR(OCI_IPC_COLLECTION, coll);
    CHECK_PTR(OCI_IPC_VOID,       size);

    err = OcilibErrorGet(ocilib_call_context.env, TRUE, TRUE);

    if (NULL != str)
    {
        *str = 0;
    }

    len += OcilibStringAddToBuffer(str, len, coll->typinf->name,
                                   (OCI_UInt32)ostrlen(coll->typinf->name), FALSE);

    len += OcilibStringAddToBuffer(str, len, OTEXT("("), 1, FALSE);

    count = OcilibCollectionGetSize(coll);

    for (OCI_UInt32 i = 0; i < count; i++)
    {
        OCI_Elem* elem = OcilibCollectionGetElement(coll, i);

        quote = TRUE;

        if (OcilibElementIsNull(elem))
        {
            len += OcilibStringAddToBuffer(str, len, OCI_STRING_NULL, OCI_STRING_NULL_SIZE, FALSE);
        }
        else
        {
            void            *data      = NULL;
            OCI_UInt32       data_size = 0;
            const OCI_UInt32 data_type = coll->typinf->cols[0].datatype;

            otext* tmpbuf = NULL;

            switch (data_type)
            {
                case OCI_CDT_TEXT:
                {
                    OCI_CALL(data_size, OCIStringSize, coll->con->env->handle, elem->handle);
                    data      = (void*)OcilibElementGetString(elem);
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
                        OCI_CALL(data_size, OCIRawSize, coll->con->env->handle, (OCIRaw *) data);
                        OCI_CALL(data, OCIRawPtr, coll->con->env->handle, (OCIRaw *) data);
                    }
                    break;
                }
                case OCI_CDT_DATETIME:
                {
                    data = (void*)OcilibElementGetDate(elem);
                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    data = (void*)OcilibElementGetTimestamp(elem);
                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    data = (void*)OcilibElementGetInterval(elem);
                    break;
                }
                case OCI_CDT_LOB:
                {
                    data = (void*)OcilibElementGetLob(elem);
                    break;
                }
                case OCI_CDT_FILE:
                {
                    data = (void*)OcilibElementGetFile(elem);
                    break;
                }
                case OCI_CDT_REF:
                {
                    data = (void*)OcilibElementGetReference(elem);
                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    data  = (void*)OcilibElementGetObject(elem);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    data  = (void*)OcilibElementGetCollection(elem);
                    quote = FALSE;
                }
            }

            CHECK(NULL != data || OCI_CDT_TEXT == data_type);
            CHECK_ERROR(err);

            tmpbuf = str;

            if (NULL != tmpbuf)
            {
                tmpbuf += len;
            }

            if (NULL != data)
            {
                len += OcilibStringGetFromType(coll->con,
                                               &coll->typinf->cols[0],
                                               data,
                                               data_size,
                                               tmpbuf,
                                               tmpbuf && size ? *size - len : 0,
                                               quote);
            }
            else
            {
                len += OcilibStringAddToBuffer(str, len, OCI_STRING_NULL,
                                               OCI_STRING_NULL_SIZE, FALSE);
            }

            CHECK(NULL == err || OCI_UNKNOWN == err->type);
        }

        if (i + 1 < count)
        {
            len += OcilibStringAddToBuffer(str, len, OTEXT(", "), 2, FALSE);
        }
    }

    len += OcilibStringAddToBuffer(str, len, OTEXT(")"), 1, FALSE);

    *size = len;

    SET_SUCCESS();

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
    );
}
