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
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollInit
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_CollInit
(
    OCI_Connection *con,
    OCI_Coll      **pcoll,
    void           *handle,
    OCI_TypeInfo   *typinf
)
{
    OCI_Coll *coll = NULL;
    boolean   res  = FALSE;

    OCI_CHECK(NULL == pcoll, NULL)

    if (!*pcoll)
    {
        *pcoll = (OCI_Coll *) OCI_MemAlloc(OCI_IPC_COLLECTION, sizeof(*coll), (size_t) 1, TRUE);
    }

    if (*pcoll)
    {
        res = TRUE;

        coll = *pcoll;

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

            OCI_CALL2
            (
                res, con,

                OCI_ObjectNew(coll->con->env, coll->con->err, coll->con->cxt,
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

    if (!res && coll)
    {
        OCI_CollFree(coll);
        *pcoll = coll = NULL;
    }

    return coll;
}

/* ********************************************************************************************* *
 *                             PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_API OCI_CollCreate
(
    OCI_TypeInfo *typinf
)
{
    OCI_LIB_CALL_ENTER(OCI_Coll *, NULL)

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)

    call_retval = OCI_CollInit(typinf->con, &call_retval, (OCIColl *) NULL, typinf);
    call_status = (NULL != call_retval);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollFree
(
    OCI_Coll *coll
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CHECK_OBJECT_FETCHED(coll)

    /* free data element */

    if (coll->elem)
    {
        coll->elem->hstate = OCI_OBJECT_FETCHED_DIRTY;
        OCI_ElemFree(coll->elem);
        coll->elem = NULL;
    }

    /* free collection for local object */

    if ((OCI_OBJECT_ALLOCATED == coll->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == coll->hstate))
    {
        OCI_OCIObjectFree(coll->con->env, coll->typinf->con->err, coll->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != coll->hstate)
    {
        OCI_FREE(coll)
    }

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Coll ** OCI_API OCI_CollArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    OCI_LIB_CALL_ENTER(OCI_Coll **, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_COLLECTION, 0, sizeof(OCIColl *),
                          sizeof(OCI_Coll), 0, typinf);

    if (arr)
    {
        call_retval = (OCI_Coll **) arr->tab_obj;
        call_status = TRUE;
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollArrayFree
(
    OCI_Coll **colls
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ARRAY, colls)

    call_retval = call_status = OCI_ArrayFreeFromHandles((void **)colls);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollAssign
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollAssign
(
    OCI_Coll *coll,
    OCI_Coll *coll_src
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll_src)
    OCI_CHECK_COMPAT(coll->con, coll->typinf->cols[0].libcode == coll_src->typinf->cols[0].libcode)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, coll->con,

        OCICollAssign(coll->con->env, coll->con->err, coll_src->handle, coll->handle)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_CollGetType
(
    OCI_Coll *coll
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)

    call_status = TRUE;

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        call_retval = OCI_COLL_NESTED_TABLE;
    }
    else if (OCI_TYPECODE_VARRAY == coll->typinf->colcode)
    {
        call_retval = OCI_COLL_VARRAY;
    }

#if OCI_VERSION_COMPILE >= OCI_12_1

    else if (OCI_TYPECODE_ITABLE == coll->typinf->colcode)
    {
        call_retval = OCI_COLL_INDEXED_TABLE;
    }

#endif

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollGetMax
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_CollGetMax
(
    OCI_Coll *coll
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)

    call_retval = (unsigned int) OCICollMax(coll->con->env, coll->handle);
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_CollGetSize
(
    OCI_Coll *coll
)
{
    sb4 value = 0;

    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, coll->con,

        OCICollSize(coll->con->env, coll->con->err, coll->handle, &value)
    )

    call_retval = (unsigned int)value;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollTrim
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollTrim
(
    OCI_Coll     *coll,
    unsigned int  nb_elem
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CHECK_BOUND(coll->con, (sb4) nb_elem, (sb4) 0, (sb4) OCI_CollGetSize(coll));

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, coll->con,

        OCICollTrim(coll->con->env, coll->con->err, (sb4) nb_elem, coll->handle)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollGetElem
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * OCI_API OCI_CollGetElem
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    boolean  exists = FALSE;
    void    *data   = NULL;
    OCIInd  *p_ind  = NULL;

    OCI_LIB_CALL_ENTER(OCI_Elem*, NULL)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, coll->con,

        OCICollGetElem(coll->con->env, coll->con->err, coll->handle, (sb4) index-1,
                       &exists, &data, (dvoid **) (dvoid *) &p_ind)
    )

    if (call_status && exists && data)
    {
        call_retval = coll->elem = OCI_ElemInit(coll->con, &coll->elem, data, p_ind, coll->typinf);
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollGetElem2
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollGetElem2
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
)
{
    boolean  exists = FALSE;
    void    *data   = NULL;
    OCIInd  *p_ind  = NULL;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CHECK_COMPAT(coll->con, elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, coll->con,

        OCICollGetElem(coll->con->env, coll->con->err, coll->handle, (sb4) index-1,
                       &exists, &data, (dvoid **) (dvoid *) &p_ind)
    )

    if (call_status && exists && data)
    {
        call_status = (NULL != OCI_ElemInit(coll->con, &elem, data, p_ind, coll->typinf));
    }
    else
    {
        call_status = OCI_ElemSetNullIndicator(elem, OCI_IND_NULL);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollSetElem
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollSetElem
(
    OCI_Coll     *coll,
    unsigned int  index,
    OCI_Elem     *elem
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CHECK_COMPAT(coll->con, elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, coll->con,

        OCICollAssignElem(coll->con->env, coll->con->err, (sb4) index-1, 
                          elem->handle, elem->pind, coll->handle)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollAppend
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollAppend
(
    OCI_Coll *coll,
    OCI_Elem *elem
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CHECK_COMPAT(coll->con, elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, coll->con,

        OCICollAppend(coll->con->env, coll->con->err, elem->handle, elem->pind, coll->handle)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OCI_API OCI_CollGetTypeInfo
(
    OCI_Coll *coll
)
{
    OCI_LIB_CALL_ENTER(OCI_TypeInfo *, NULL)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)

    call_retval = coll->typinf;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollClear
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollClear
(
    OCI_Coll *coll
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)

    call_retval = call_status = OCI_CollTrim(coll, OCI_CollGetSize(coll));

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollDeleteElem
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollDeleteElem
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)

    call_status = TRUE;

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        OCI_CALL2
        (
            call_status, coll->con,

            OCITableDelete(coll->con->env, coll->con->err, (sb4) index-1, coll->handle)
        )
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollGetCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_CollGetCount
(
    OCI_Coll *coll
)
{
    sb4 value = 0;

    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)

    call_status = TRUE;

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        OCI_CALL2
        (
            call_status, coll->con,

            OCITableSize(coll->con->env, coll->con->err, coll->handle, &value)
        )
    }
    else
    {
        OCI_CALL2
        (
            call_status, coll->con,

            OCICollSize(coll->con->env, coll->con->err, coll->handle, &value)
        )
    }

    call_retval = (unsigned int)value;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollToText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollToText
(
    OCI_Coll     *coll,
    unsigned int *size,
    otext        *str
)
{
    boolean      quote  = TRUE;
    unsigned int len    = 0;

    int i, n;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CHECK_PTR(OCI_IPC_VOID, size)

    call_status = TRUE;

    if (str)
    {
        *str = 0;
    }

    len += OCI_StringAddToBuffer(str, len, coll->typinf->name, FALSE);
    len += OCI_StringAddToBuffer(str, len, OTEXT("("), FALSE);

    n = OCI_CollGetSize(coll);

    for (i = 1; i <= n; i++)
    {
        OCI_Elem *elem = OCI_CollGetAt(coll, i);

        quote = TRUE;

        if (OCI_ElemIsNull(elem))
        {
            len += OCI_StringAddToBuffer(str, len, OCI_STRING_NULL, FALSE);
        }
        else
        {
            void    *data       = NULL;
            unsigned data_size  = 0;

            switch (coll->typinf->cols[0].datatype)
            {
                case OCI_CDT_TEXT:
                {
                    data  = (void *) OCI_ElemGetString(elem);
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
                    data = (void *)elem->handle;
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
                    data  = (void *) OCI_ElemGetDate(elem);
                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    data  = (void *) OCI_ElemGetTimestamp(elem);
                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    data  = (void *) OCI_ElemGetInterval(elem);
                    break;
                }
                case OCI_CDT_LOB:
                {
                    data  = (void *) OCI_ElemGetLob(elem);
                    break;
                }
                case OCI_CDT_FILE:
                {
                    data  = (void *) OCI_ElemGetFile(elem);
                    break;
                }
                case OCI_CDT_REF:
                {
                    data  = (void *) OCI_ElemGetRef(elem);
                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    data  = (void *) OCI_ElemGetObject(elem);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    data  =  (void *) OCI_ElemGetColl(elem);
                    quote = FALSE;
                }
            }

            call_status = (NULL != data);

            if (call_status)
            {
                otext *tmpbuf = str;

                if (tmpbuf)
                {
                    tmpbuf += len;
                }

                len += OCI_StringGetFromType(coll->con, &coll->typinf->cols[0], data, data_size, tmpbuf, quote);
            }
            else
            {
                break;
            }
        }

        if (i < n)
        {
            len += OCI_StringAddToBuffer(str, len, OTEXT(", "), FALSE);
        }
    }

    if (call_status)
    {
        len += OCI_StringAddToBuffer(str, len, OTEXT(")"), FALSE);

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

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}
