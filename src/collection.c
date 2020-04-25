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
    DECLARE_CTX(TRUE)
    CALL_CONTEXT_FROM_CON(con)

    ALLOC_DATA(OCI_IPC_COLLECTION, coll, 1);

    if (STATUS)
    {
        coll->con    = con;
        coll->handle = handle;
        coll->typinf = typinf;

        if (!coll->handle || (OCI_OBJECT_ALLOCATED_ARRAY == coll->hstate))
        {
            /* allocates handle for non fetched collection */

            if (OCI_OBJECT_ALLOCATED_ARRAY != coll->hstate)
            {
                coll->hstate = OCI_OBJECT_ALLOCATED;
            }

            EXEC
            (
                MemoryAllocateObject(coll->con->env, coll->con->err, coll->con->cxt,
                                     typinf->colcode, typinf->tdo, (void *) NULL,
                                     OCI_DURATION_SESSION, TRUE, (dvoid **) &coll->handle)
            )
        }
        else
        {
            coll->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }

    /* check for failure */

    if (!STATUS && coll)
    {
        CollectionFree(coll);
        coll = NULL;
    }

    return coll;
}

/* --------------------------------------------------------------------------------------------- *
 * CollCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * CollectionCreate
(
    OCI_TypeInfo *typinf
)
{
    CALL_ENTER(OCI_Coll *, NULL)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CALL_CONTEXT_FROM_CON(typinf->con)

    RETVAL = CollectionInitialize(typinf->con, NULL, NULL, typinf);
    STATUS = (NULL != RETVAL);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollFree
 * --------------------------------------------------------------------------------------------- */

boolean CollectionFree
(
    OCI_Coll *coll
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CHECK_OBJECT_FETCHED(coll)
    CALL_CONTEXT_FROM_CON(coll->con)

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

    RETVAL = TRUE;

    CALL_EXIT()
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
    OCI_Array *arr = NULL;

    CALL_ENTER(OCI_Coll **, NULL)
    CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    CALL_CONTEXT_FROM_CON(con)

    arr = ArrayCreate(con, nbelem, OCI_CDT_COLLECTION, 0, sizeof(OCIColl *), sizeof(OCI_Coll), 0, typinf);

    STATUS = (arr != NULL);

    if (STATUS)
    {
        RETVAL = (OCI_Coll **) arr->tab_obj;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean CollectionFreeArray
(
    OCI_Coll **colls
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ARRAY, colls)

    RETVAL = STATUS = ArrayFreeFromHandles((void **)colls);

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll_src)
    CALL_CHECK_COMPAT(coll->con, coll->typinf->cols[0].libcode == coll_src->typinf->cols[0].libcode)
    CALL_CONTEXT_FROM_CON(coll->con)

    EXEC(OCICollAssign(coll->con->env, coll->con->err, coll_src->handle, coll->handle))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int CollectionGetType
(
    OCI_Coll *coll
)
{
    CALL_ENTER(unsigned int, OCI_UNKNOWN)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CONTEXT_FROM_CON(coll->con)

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        RETVAL = OCI_COLL_NESTED_TABLE;
    }
    else if (OCI_TYPECODE_VARRAY == coll->typinf->colcode)
    {
        RETVAL = OCI_COLL_VARRAY;
    }

#if OCI_VERSION_COMPILE >= OCI_12_1

    else if (OCI_TYPECODE_ITABLE == coll->typinf->colcode)
    {
        RETVAL = OCI_COLL_INDEXED_TABLE;
    }

#endif

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetMax
 * --------------------------------------------------------------------------------------------- */

unsigned int CollectionGetMax
(
    OCI_Coll *coll
)
{
    CALL_ENTER(unsigned int, 0)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CONTEXT_FROM_CON(coll->con)

    RETVAL = (unsigned int) OCICollMax(coll->con->env, coll->handle);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int CollectionGetSize
(
    OCI_Coll *coll
)
{
    sb4 value = 0;

    CALL_ENTER(unsigned int, 0)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CONTEXT_FROM_CON(coll->con)

    EXEC(OCICollSize(coll->con->env, coll->con->err, coll->handle, &value))

    RETVAL = (unsigned int)value;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CHECK_BOUND(coll->con, (sb4) nb_elem, (sb4) 0, (sb4) CollectionGetSize(coll));
    CALL_CONTEXT_FROM_CON(coll->con)

    EXEC(OCICollTrim(coll->con->env, coll->con->err, (sb4) nb_elem, coll->handle))

    RETVAL = STATUS;

    CALL_EXIT()
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
    boolean exists = FALSE;
    void   *data   = NULL;
    OCIInd *p_ind  = NULL;

    CALL_ENTER(OCI_Elem*, NULL)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CONTEXT_FROM_CON(coll->con)

    EXEC(OCICollGetElem(coll->con->env, coll->con->err, coll->handle, (sb4) index-1, &exists, &data, (dvoid **) (dvoid *) &p_ind))

    if (STATUS && exists && data)
    {
        RETVAL = coll->elem = ElementInitialize(coll->con, coll->elem, data, p_ind, coll->typinf);
    }

    CALL_EXIT()
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
    boolean exists = FALSE;
    void   *data   = NULL;
    OCIInd *p_ind  = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CHECK_COMPAT(coll->con, elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype)
    CALL_CONTEXT_FROM_CON(coll->con)

    EXEC(OCICollGetElem(coll->con->env, coll->con->err, coll->handle, (sb4) index-1, &exists, &data, (dvoid **) (dvoid *) &p_ind))

    if (STATUS && exists && data)
    {
        elem = ElementInitialize(coll->con, elem, data, p_ind, coll->typinf);

        STATUS = (NULL != elem);
    }
    else
    {
        STATUS = ElementSetNullIndicator(elem, OCI_IND_NULL);
    }

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CHECK_COMPAT(coll->con, elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype)
    CALL_CONTEXT_FROM_CON(coll->con)

    EXEC(OCICollAssignElem(coll->con->env, coll->con->err, (sb4) index-1, elem->handle, elem->pind, coll->handle))

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CHECK_COMPAT(coll->con, elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype)
    CALL_CONTEXT_FROM_CON(coll->con)

    EXEC(OCICollAppend(coll->con->env, coll->con->err, elem->handle, elem->pind, coll->handle))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * CollectionGetTypeInfo
(
    OCI_Coll *coll
)
{
    GET_PROP(OCI_TypeInfo*, NULL, OCI_IPC_COLLECTION, coll, typinf, coll->con, NULL, coll->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * CollClear
 * --------------------------------------------------------------------------------------------- */

boolean CollectionClear
(
    OCI_Coll *coll
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CONTEXT_FROM_CON(coll->con)

    RETVAL = STATUS = CollectionTrim(coll, CollectionGetSize(coll));

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CONTEXT_FROM_CON(coll->con)

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        EXEC(OCITableDelete(coll->con->env, coll->con->err, (sb4) index-1, coll->handle))
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetCount
 * --------------------------------------------------------------------------------------------- */

unsigned int CollectionGetCount
(
    OCI_Coll *coll
)
{
    sb4 value = 0;

    CALL_ENTER(unsigned int, 0)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CONTEXT_FROM_CON(coll->con)

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        EXEC(OCITableSize(coll->con->env, coll->con->err, coll->handle, &value))
    }
    else
    {
        EXEC(OCICollSize(coll->con->env, coll->con->err, coll->handle, &value))
    }

    RETVAL = (unsigned int) value;

    CALL_EXIT()
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
    OCI_Error   *err   = NULL;
    boolean      quote = TRUE;
    unsigned int len   = 0;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    CALL_CHECK_PTR(OCI_IPC_VOID, size)
    CALL_CONTEXT_FROM_CON(coll->con)

    err = ErrorGet(TRUE, TRUE);

    if (str)
    {
        *str = 0;
    }

    len += StringAddToBuffer(str, len, coll->typinf->name, (unsigned int) ostrlen(coll->typinf->name), FALSE);
    len += StringAddToBuffer(str, len, OTEXT("("), 1, FALSE);

    const unsigned int n = CollectionGetSize(coll);

    for (unsigned int i = 1; i <= n && STATUS; i++)
    {
        OCI_Elem *elem = CollectionGetElement(coll, i);

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

            switch (data_type)
            {
                case OCI_CDT_TEXT:
                {
                    data_size = OCIStringSize(Env.env, elem->handle);
                    data      = (void *) ElementGetString(elem);
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
                    data = (void *) ElementGetDate(elem);
                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    data = (void *) ElementGetTimestamp(elem);
                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    data = (void *) ElementGetInterval(elem);
                    break;
                }
                case OCI_CDT_LOB:
                {
                    data = (void *) ElementGetLob(elem);
                    break;
                }
                case OCI_CDT_FILE:
                {
                    data = (void *) ElementGetFile(elem);
                    break;
                }
                case OCI_CDT_REF:
                {
                    data = (void *) ElementGetReference(elem);
                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    data  = (void *) ElementGetObject(elem);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    data  =  (void *) ElementGetCollection(elem);
                    quote = FALSE;
                }
            }

            STATUS = (NULL != data || OCI_CDT_TEXT == data_type) && (NULL == err || !err->raise);

            if (STATUS)
            {
                otext *tmpbuf = str;

                if (tmpbuf)
                {
                    tmpbuf += len;
                }

                if (data)
                {
                    len += StringGetFromType(coll->con, &coll->typinf->cols[0], data, data_size, tmpbuf,
                                             tmpbuf && size ? *size - len : 0, quote);
                }
                else
                {
                    len += StringAddToBuffer(str, len, OCI_STRING_NULL, OCI_STRING_NULL_SIZE, FALSE);
                }
                STATUS = (NULL == err || OCI_UNKNOWN == err->type);
            }
        }

        if (STATUS && i < n)
        {
            len += StringAddToBuffer(str, len, OTEXT(", "), 2, FALSE);
        }
    }

    if (STATUS)
    {
        len += StringAddToBuffer(str, len, OTEXT(")"), 1, FALSE);

        *size = len;
    }
    else
    {
        *size = 0;

        if (str)
        {
            *str = 0;
        }
    }

    RETVAL = STATUS;

    CALL_EXIT()
}
