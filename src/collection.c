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

#include "collection.h"

#include "array.h"
#include "connection.h"
#include "element.h"
#include "macros.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Collection * OcilibCollectionInitialize
(
    OCI_Connection *con,
    OCI_Collection *coll,
    void           *handle,
    OCI_TypeInfo   *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Collection*, coll,
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
        if (FAILURE && NULL != coll)
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

OCI_Collection * OcilibCollectionCreate
(
    OCI_TypeInfo *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Collection*, NULL,
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
    OCI_Collection *coll
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
 * OcilibCollectionArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Collection ** OcilibCollectionArrayCreate
(
    OCI_TypeInfo *typinf,
    OCI_UInt32    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Collection**, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);

    arr = OcilibArrayCreate(typinf->con->env, typinf->con, nbelem, OCI_CDT_COLLECTION, 0,
                            sizeof(OCIColl*), sizeof(OCI_Collection), 0, typinf);

    CHECK_NULL(arr);

    SET_RETVAL((OCI_Collection **) arr->tab_obj);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionArrayFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionArrayFree
(
    OCI_Collection **colls
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, NULL
    );

    CHECK_PTR(OCI_IPC_ARRAY, colls);

    OCI_Collection *col = colls[0];

    CHECK_PTR(OCI_IPC_COLLECTION, col);

    SET_RETVAL(OcilibArrayFreeFromHandles(col->con->env, (void **)colls));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionAssign
(
    OCI_Collection *coll,
    OCI_Collection *coll_src
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
    OCI_Collection *coll
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
    OCI_Collection *coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);

    sb4 coll_max = 0;

    OCI_CALL(coll_max, OCICollMax, coll->con->env->handle, coll->handle);

    SET_RETVAL((OCI_UInt32) coll_max);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionGetSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibCollectionGetSize
(
    OCI_Collection *coll
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
    OCI_Collection *coll,
    OCI_UInt32      nb_elem
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
 * OcilibCollectionGetElementByIndex
 * --------------------------------------------------------------------------------------------- */

OCI_Element * OcilibCollectionGetElementByIndex
(
    OCI_Collection *coll,
    OCI_UInt32      index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Element*, NULL,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);


    /* reject string-indexed associative arrays: index semantics are
       undefined for them (use the ByKey API instead) */

    CHECK_COMPAT(OCI_CKT_STRING != OcilibCollectionGetKeyType(coll));

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

    OCI_Element* elem = NULL;

    if (exists && NULL != data)
    {
        elem = coll->elem = OcilibElementInitialize(coll->elem, data, ind, coll->typinf);
        CHECK_NULL(elem);
    }

    SET_RETVAL(elem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionSetElementByIndex
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionSetElementByIndex
(
    OCI_Collection *coll,
    OCI_UInt32      index,
    OCI_Element    *elem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);


    /* reject string-indexed associative arrays: index semantics are
       undefined for them (use the ByKey API instead) */

    CHECK_COMPAT(OCI_CKT_STRING != OcilibCollectionGetKeyType(coll));
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
 * OcilibCollectionAppend
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionAppend
(
    OCI_Collection *coll,
    OCI_Element    *elem
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
    OCI_Collection *coll
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
    OCI_Collection *coll
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
 * OcilibCollectionDeleteElementByIndex
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionDeleteElementByIndex
(
    OCI_Collection *coll,
    OCI_UInt32      index
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);


    /* reject string-indexed associative arrays: index semantics are
       undefined for them (use the ByKey API instead) */

    CHECK_COMPAT(OCI_CKT_STRING != OcilibCollectionGetKeyType(coll));

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
 * OcilibCollectionGetKeyType
 * --------------------------------------------------------------------------------------------- */

OCI_CollKeyType OcilibCollectionGetKeyType
(
    OCI_Collection *coll
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_CollKeyType, OCI_CKT_UNKNOWN,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);

    OCI_CollKeyType result = OCI_CKT_UNKNOWN;

    if (OCI_TYPECODE_ITABLE == coll->typinf->colcode)
    {
        /* Numeric-indexed associative arrays are the only index-by tables
           pre-23.6 clients can manipulate; and on 23.6+ clients,
           OCICollGetKeytype() errors on numeric-indexed tables anyway */

        result = OCI_CKT_NUMBER;

        if (OcilibConnectionIsVersionSupported(coll->con, OCI_23_6))
        {
            ub2   keytype = 0;
            sword ret     = OCI_ERROR;

            OCI_CALL
            (
                ret,
                OCICollGetKeytype,
                coll->con->env->handle, coll->con->err,
                coll->handle, &keytype
            );

            if (OCI_SUCCESSFUL(ret))
            {
                /* OCI_KEYTYPE_* values map 1:1 to OCI_CKT_* (STRING=1, SB4=2) —
                   a direct cast avoids referencing constants that may be
                   absent when compiling against older Oracle SDKs */

                result = (OCI_CollKeyType) keytype;
            }
            else
            {
                /* drain the expected OCI-21560 on numeric-indexed tables so
                   OCI_GetLastError() does not report a spurious error */

                sb4 errcode = 0;

                OCI_ERROR_GET(coll->con->err, &errcode, NULL, 0);
            }
        }
    }

    SET_RETVAL(result);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionKeyExists
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionKeyExists
(
    OCI_Collection *coll,
    const otext    *key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    OCI_Boolean exists = FALSE;

    CHECK_PTR(OCI_IPC_COLLECTION, coll);
    CHECK_PTR(OCI_IPC_STRING,     key);
    CHECK_ENV_FEATURE(OCI_FEATURE_COLL_KEY, OCI_23_6);

    /* string-keyed access only valid on INDEX BY VARCHAR2 collections */

    CHECK_COMPAT(OCI_CKT_STRING == OcilibCollectionGetKeyType(coll));

    CHECK_OCI
    (
        coll->con->err,
        OCITableKeyExists,
        coll->con->env->handle, coll->con->err,
        (const dbtext*) key, (ub4) OcilibStringDBSize(key),
        coll->handle, (boolean*) &exists
    );

    SET_RETVAL(exists);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionGetElementByKey
 * --------------------------------------------------------------------------------------------- */

OCI_Element * OcilibCollectionGetElementByKey
(
    OCI_Collection *coll,
    const otext    *key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Element*, NULL,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    void   *data  = NULL;
    OCIInd  ind   = OCI_IND_NULL;
    void   *pind  = &ind;

    CHECK_PTR(OCI_IPC_COLLECTION, coll);
    CHECK_PTR(OCI_IPC_STRING,     key);
    CHECK_ENV_FEATURE(OCI_FEATURE_COLL_KEY, OCI_23_6);

    /* string-keyed access only valid on INDEX BY VARCHAR2 collections */

    CHECK_COMPAT(OCI_CKT_STRING == OcilibCollectionGetKeyType(coll));

    /* OCICollKeyGetElem() requires 'elemind' to point to caller-provided
       OCIInd storage: it writes through the pointer (unlike OCICollGetElem()
       which returns a pointer to OCI-owned indicator storage) */

    CHECK_OCI
    (
        coll->con->err,
        OCICollKeyGetElem,
        coll->con->env->handle, coll->con->err,
        coll->handle, (const dbtext*) key,
        (ub4) OcilibStringDBSize(key), &data, &pind
    );

    OCI_Element* elem = NULL;

    if (NULL != data)
    {
        elem = coll->elem = OcilibElementInitialize(coll->elem, data, NULL, coll->typinf);
        CHECK_NULL(elem);

        *elem->pind = *((OCIInd *) pind);
    }

    SET_RETVAL(elem);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionSetElementByKey
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionSetElementByKey
(
    OCI_Collection *coll,
    const otext    *key,
    OCI_Element    *elem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);
    CHECK_PTR(OCI_IPC_STRING,     key);
    CHECK_PTR(OCI_IPC_ELEMENT,    elem);
    CHECK_ENV_FEATURE(OCI_FEATURE_COLL_KEY, OCI_23_6);

    /* string-keyed access only valid on INDEX BY VARCHAR2 collections */

    CHECK_COMPAT(OCI_CKT_STRING == OcilibCollectionGetKeyType(coll));
    CHECK_COMPAT(elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype);

    CHECK_OCI
    (
        coll->con->err,
        OCICollKeyAssignElem,
        coll->con->env->handle, coll->con->err,
        (const dbtext*) key, (ub4) OcilibStringDBSize(key),
        elem->handle, elem->pind, coll->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionDeleteElementByKey
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionDeleteElementByKey
(
    OCI_Collection *coll,
    const otext    *key
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_COLLECTION, coll
    );

    CHECK_PTR(OCI_IPC_COLLECTION, coll);
    CHECK_PTR(OCI_IPC_STRING,     key);
    CHECK_ENV_FEATURE(OCI_FEATURE_COLL_KEY, OCI_23_6);

    /* string-keyed access only valid on INDEX BY VARCHAR2 collections */

    CHECK_COMPAT(OCI_CKT_STRING == OcilibCollectionGetKeyType(coll));

    CHECK_OCI
    (
        coll->con->err,
        OCITableKeyDelete,
        coll->con->env->handle, coll->con->err,
        (const dbtext*) key, (ub4) OcilibStringDBSize(key),
        coll->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}








/* --------------------------------------------------------------------------------------------- *
 * OcilibCollectionGetCount
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibCollectionGetCount
(
    OCI_Collection *coll
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
 * OcilibCollectionToText
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibCollectionToText
(
    OCI_Collection *coll,
    OCI_UInt32     *size,
    otext          *str
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
        OCI_Element* elem = OcilibCollectionGetElementByIndex(coll, i);

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
                    data = (void*)OcilibElementGetString(elem);
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
