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
 * $Id: collection.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

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
    boolean res    = FALSE;

    OCI_CHECK(pcoll == NULL, NULL);

    if (!*pcoll)
    {
        *pcoll = (OCI_Coll *) OCI_MemAlloc(OCI_IPC_COLLECTION, sizeof(*coll), (size_t) 1, TRUE);
    }

    if (*pcoll)
    {
        coll = *pcoll;

        res = TRUE;

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

    if (!res)
    {
        OCI_CollFree(coll);
        coll = NULL;
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
    OCI_Coll *coll = NULL;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);
    OCI_CHECK(typinf->colcode == OCI_UNKNOWN, NULL)

    coll = OCI_CollInit(typinf->con, &coll, (OCIColl *) NULL, typinf);

    OCI_RESULT(coll != NULL);

    return coll;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollFree
(
    OCI_Coll *coll
)
{
    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, FALSE);
    OCI_CHECK_OBJECT_FETCHED(coll, FALSE);

    /* free data element accessor */

    if (coll->elem)
    {
        coll->elem->hstate = OCI_OBJECT_FETCHED_DIRTY;
        OCI_ElemFree(coll->elem);
        coll->elem = NULL;
    }

    /* free collection for local object */

    if ((OCI_OBJECT_ALLOCATED == coll->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == coll->hstate))
    {
        OCI_OCIObjectFree(coll->con->env, coll->typinf->con->err, coll->handle, OCI_OBJECTFREE_NONULL);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != coll->hstate)
    {
        OCI_FREE(coll);
    }

    OCI_RESULT(TRUE);

    return TRUE;
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
    OCI_Array *arr   = NULL;
    OCI_Coll **colls = NULL;

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_COLLECTION, 0, sizeof(OCIColl *),
                          sizeof(OCI_Coll), 0, typinf);

    if (arr)
    {
        colls = (OCI_Coll **) arr->tab_obj;
    }

    return colls;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollArrayFree
(
    OCI_Coll **colls
)
{
    return OCI_ArrayFreeFromHandles((void **) colls);
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll_src, FALSE);

    OCI_CHECK_COMPAT(coll->con, coll->typinf->cols[0].libcode == coll_src->typinf->cols[0].libcode, FALSE);

    OCI_CALL2
    (
        res, coll->con,

        OCICollAssign(coll->con->env, coll->con->err, coll_src->handle, coll->handle)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_CollGetType
(
    OCI_Coll *coll
)
{
    unsigned int type = OCI_UNKNOWN;

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, OCI_UNKNOWN);

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        type = OCI_COLL_NESTED_TABLE;
    }
    else if (OCI_TYPECODE_VARRAY == coll->typinf->colcode)
    {
        type = OCI_COLL_VARRAY;
    }

    OCI_RESULT(TRUE);

    return type;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollGetMax
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_CollGetMax
(
    OCI_Coll *coll
)
{
    int max = 0;

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, 0);

    max = OCICollMax(coll->con->env, coll->handle);

    OCI_RESULT(TRUE);

    return (unsigned int) max;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_CollGetSize
(
    OCI_Coll *coll
)
{
    boolean res = TRUE;
    sb4 size    = 0;

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, 0);

    OCI_CALL2
    (
        res, coll->con,

        OCICollSize(coll->con->env, coll->con->err, coll->handle, &size)
    )

    OCI_RESULT(res);

    return (unsigned int) size;
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
    boolean res = TRUE;
    unsigned int size = 0;

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, FALSE);

    size = OCI_CollGetSize(coll);

    OCI_CHECK_BOUND(coll->con, (sb4) nb_elem, (sb4) 0, (sb4) size, FALSE);

    OCI_CALL2
    (
        res, coll->con,

        OCICollTrim(coll->con->env, coll->con->err, (sb4) nb_elem, coll->handle)
    )

    OCI_RESULT(res);

    return res;
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
    boolean res    = TRUE;
    boolean exists = FALSE;
    void *data     = NULL;
    OCIInd *p_ind  = NULL;
    OCI_Elem *elem = NULL;

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, NULL);

    OCI_CALL2
    (
        res, coll->con,

        OCICollGetElem(coll->con->env, coll->con->err, coll->handle, (sb4) index-1,
                       &exists, &data, (dvoid **) (dvoid *) &p_ind)
    )

    if (res && exists && data)
    {
        elem = coll->elem = OCI_ElemInit(coll->con, &coll->elem, data, p_ind, coll->typinf);
    }

    OCI_RESULT(res);

    return elem;
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
    boolean res    = TRUE;
    boolean exists = FALSE;
    void *data     = NULL;
    OCIInd *p_ind  = NULL;

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, FALSE);
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);

    OCI_CHECK_COMPAT(coll->con, elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype, FALSE);

    OCI_CALL2
    (
        res, coll->con,

        OCICollGetElem(coll->con->env, coll->con->err, coll->handle, (sb4) index-1,
                       &exists, &data, (dvoid **) (dvoid *) &p_ind)
    )

    if (res && exists && data)
    {
        res = (OCI_ElemInit(coll->con, &elem, data, p_ind, coll->typinf) != NULL);
    }
    else
    {
        OCI_ElemSetNullIndicator(elem, OCI_IND_NULL);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollSetElem
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollSetElem
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, FALSE);
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);

    OCI_CHECK_COMPAT(coll->con, elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype, FALSE);

    OCI_CALL2
    (
        res, coll->con,

        OCICollAssignElem(coll->con->env, coll->con->err, (sb4) index-1, elem->handle,
                          elem->pind, coll->handle)
    )

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, FALSE);
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);

    OCI_CHECK_COMPAT(coll->con, elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype, FALSE);

    OCI_CALL2
    (
        res, coll->con,

        OCICollAppend(coll->con->env, coll->con->err, elem->handle, elem->pind, coll->handle)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OCI_API OCI_CollGetTypeInfo
(
    OCI_Coll *coll
)
{
    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, NULL);

    OCI_RESULT(TRUE);

    return coll->typinf;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollClear
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_CollClear
(
    OCI_Coll *coll
)
{
    boolean res = TRUE;

    unsigned int size = OCI_CollGetSize(coll);

    if (size > 0)
    {
        res = OCI_CollTrim(coll, size);
    }

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;
    boolean ret = FALSE;

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, FALSE);

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        OCI_CALL2
        (
            res, coll->con,

            OCITableDelete(coll->con->env, coll->con->err, (sb4) index-1, coll->handle)
        )

        ret = res;
    }

    OCI_RESULT(res);

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollGetCount
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_CollGetCount
(
    OCI_Coll *coll
)
{
    boolean res = TRUE;
    sb4 count   = 0;

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, 0);

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        OCI_CALL2
        (
            res, coll->con,

            OCITableSize(coll->con->env, coll->con->err, coll->handle, &count)
        )
    }
    else
    {
        OCI_CALL2
        (
            res, coll->con,

            OCICollSize(coll->con->env, coll->con->err, coll->handle, &count)
        )
    }

    OCI_RESULT(res);

    return (unsigned int) count;
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
    boolean      res    = TRUE;
    boolean      quote  = TRUE;
    unsigned int len    = 0;

    int i, n;

    OCI_CHECK_PTR(OCI_IPC_COLLECTION, coll, FALSE);
    OCI_CHECK_PTR(OCI_IPC_VOID, size, FALSE);

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

            res = (data != NULL);

            if (res)
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

    if (res)
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

    OCI_RESULT(res);

    return res;
}
