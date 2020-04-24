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
#include "macro.h"
#include "strings.h"

/* --------------------------------------------------------------------------------------------- *
 * CollInit
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * CollInit
(
    OCI_Connection *con,
    OCI_Coll       *coll,
    void           *handle,
    OCI_TypeInfo   *typinf
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_ALLOCATE_DATA(OCI_IPC_COLLECTION, coll, 1);

    if (OCI_STATUS)
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

            OCI_EXEC
            (
                MemObjectNew(coll->con->env, coll->con->err, coll->con->cxt,
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

    if (!OCI_STATUS && coll)
    {
        CollFree(coll);
        coll = NULL;
    }

    return coll;
}

/* --------------------------------------------------------------------------------------------- *
 * CollCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * CollCreate
(
    OCI_TypeInfo *typinf
)
{
    OCI_CALL_ENTER(OCI_Coll *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    OCI_CALL_CONTEXT_SET_FROM_CONN(typinf->con)

    OCI_RETVAL = CollInit(typinf->con, NULL, NULL, typinf);
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollFree
 * --------------------------------------------------------------------------------------------- */

boolean CollFree
(
    OCI_Coll *coll
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CALL_CHECK_OBJECT_FETCHED(coll)
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    /* free data element */

    if (coll->elem)
    {
        coll->elem->hstate = OCI_OBJECT_FETCHED_DIRTY;
        ElemFree(coll->elem);
        coll->elem = NULL;
    }

    /* free collection for local object */

    if ((OCI_OBJECT_ALLOCATED == coll->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == coll->hstate))
    {
        MemObjectFree(coll->con->env, coll->typinf->con->err, coll->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != coll->hstate)
    {
        OCI_FREE(coll)
    }

    OCI_RETVAL = TRUE;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Coll ** CollArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    OCI_CALL_ENTER(OCI_Coll **, NULL)    
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)        
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    arr = ArrayCreate(con, nbelem, OCI_CDT_COLLECTION, 0, sizeof(OCIColl *), sizeof(OCI_Coll), 0, typinf);
    OCI_STATUS = (arr != NULL);

    if (OCI_STATUS)
    {
        OCI_RETVAL = (OCI_Coll **) arr->tab_obj;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean CollArrayFree
(
    OCI_Coll **colls
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ARRAY, colls)

    OCI_RETVAL = OCI_STATUS = ArrayFreeFromHandles((void **)colls);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollAssign
 * --------------------------------------------------------------------------------------------- */

boolean CollAssign
(
    OCI_Coll *coll,
    OCI_Coll *coll_src
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll_src)
    OCI_CALL_CHECK_COMPAT(coll->con, coll->typinf->cols[0].libcode == coll_src->typinf->cols[0].libcode)
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    OCI_EXEC(OCICollAssign(coll->con->env, coll->con->err, coll_src->handle, coll->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int CollGetType
(
    OCI_Coll *coll
)
{
    OCI_CALL_ENTER(unsigned int, OCI_UNKNOWN)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        OCI_RETVAL = OCI_COLL_NESTED_TABLE;
    }
    else if (OCI_TYPECODE_VARRAY == coll->typinf->colcode)
    {
        OCI_RETVAL = OCI_COLL_VARRAY;
    }

#if OCI_VERSION_COMPILE >= OCI_12_1

    else if (OCI_TYPECODE_ITABLE == coll->typinf->colcode)
    {
        OCI_RETVAL = OCI_COLL_INDEXED_TABLE;
    }

#endif

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetMax
 * --------------------------------------------------------------------------------------------- */

unsigned int CollGetMax
(
    OCI_Coll *coll
)
{
    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    OCI_RETVAL = (unsigned int) OCICollMax(coll->con->env, coll->handle);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int CollGetSize
(
    OCI_Coll *coll
)
{
    sb4 value = 0;

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    OCI_EXEC(OCICollSize(coll->con->env, coll->con->err, coll->handle, &value))

    OCI_RETVAL = (unsigned int)value;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollTrim
 * --------------------------------------------------------------------------------------------- */

boolean CollTrim
(
    OCI_Coll     *coll,
    unsigned int  nb_elem
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CALL_CHECK_BOUND(coll->con, (sb4) nb_elem, (sb4) 0, (sb4) CollGetSize(coll));
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    OCI_EXEC(OCICollTrim(coll->con->env, coll->con->err, (sb4) nb_elem, coll->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetElem
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * CollGetElem
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    boolean  exists = FALSE;
    void    *data   = NULL;
    OCIInd  *p_ind  = NULL;

    OCI_CALL_ENTER(OCI_Elem*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    OCI_EXEC(OCICollGetElem(coll->con->env, coll->con->err, coll->handle, (sb4) index-1, &exists, &data, (dvoid **) (dvoid *) &p_ind))

    if (OCI_STATUS && exists && data)
    {
        OCI_RETVAL = coll->elem = ElemInit(coll->con, coll->elem, data, p_ind, coll->typinf);
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetElem2
 * --------------------------------------------------------------------------------------------- */

boolean CollGetElem2
(
    OCI_Coll    *coll,
    unsigned int index,
    OCI_Elem    *elem
)
{
    boolean  exists = FALSE;
    void    *data   = NULL;
    OCIInd  *p_ind  = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
	OCI_CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CALL_CHECK_COMPAT(coll->con, elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype)
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    OCI_EXEC(OCICollGetElem(coll->con->env, coll->con->err, coll->handle, (sb4) index-1, &exists, &data, (dvoid **) (dvoid *) &p_ind))

    if (OCI_STATUS && exists && data)
    {
        elem = ElemInit(coll->con, elem, data, p_ind, coll->typinf);
        OCI_STATUS = (NULL != elem);
    }
    else
    {
        OCI_STATUS = ElemSetNullIndicator(elem, OCI_IND_NULL);
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollSetElem
 * --------------------------------------------------------------------------------------------- */

boolean CollSetElem
(
    OCI_Coll     *coll,
    unsigned int  index,
    OCI_Elem     *elem
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CALL_CHECK_COMPAT(coll->con, elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype)
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    OCI_EXEC(OCICollAssignElem(coll->con->env, coll->con->err, (sb4) index-1, elem->handle, elem->pind, coll->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollAppend
 * --------------------------------------------------------------------------------------------- */

boolean CollAppend
(
    OCI_Coll *coll,
    OCI_Elem *elem
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CALL_CHECK_COMPAT(coll->con, elem->typinf->cols[0].datatype == coll->typinf->cols[0].datatype)
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    OCI_EXEC(OCICollAppend(coll->con->env, coll->con->err, elem->handle, elem->pind, coll->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * CollGetTypeInfo
(
    OCI_Coll *coll
)
{
    OCI_GET_PROP(OCI_TypeInfo*, NULL, OCI_IPC_COLLECTION, coll, typinf, coll->con, NULL, coll->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * CollClear
 * --------------------------------------------------------------------------------------------- */

boolean CollClear
(
    OCI_Coll *coll
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    OCI_RETVAL = OCI_STATUS = CollTrim(coll, CollGetSize(coll));

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollDeleteElem
 * --------------------------------------------------------------------------------------------- */

boolean CollDeleteElem
(
    OCI_Coll    *coll,
    unsigned int index
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
	OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        OCI_EXEC(OCITableDelete(coll->con->env, coll->con->err, (sb4) index-1, coll->handle))
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollGetCount
 * --------------------------------------------------------------------------------------------- */

unsigned int CollGetCount
(
    OCI_Coll *coll
)
{
    sb4 value = 0;

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    if (OCI_TYPECODE_TABLE == coll->typinf->colcode)
    {
        OCI_EXEC(OCITableSize(coll->con->env, coll->con->err, coll->handle, &value))
    }
    else
    {
        OCI_EXEC(OCICollSize(coll->con->env, coll->con->err, coll->handle, &value))
    }

    OCI_RETVAL = (unsigned int) value;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * CollToText
 * --------------------------------------------------------------------------------------------- */

boolean CollToText
(
    OCI_Coll     *coll,
    unsigned int *size,
    otext        *str
)
{
    OCI_Error *err = NULL;
    boolean quote  = TRUE;
    unsigned int len = 0;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_COLLECTION, coll)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, size)
    OCI_CALL_CONTEXT_SET_FROM_CONN(coll->con)

    err = ErrorGet(TRUE, TRUE);

    if (str)
    {
        *str = 0;
    }

    len += StringAddToBuffer(str, len, coll->typinf->name, (unsigned int) ostrlen(coll->typinf->name), FALSE);
    len += StringAddToBuffer(str, len, OTEXT("("), 1, FALSE);

    const unsigned int n = CollGetSize(coll);

    for (unsigned int i = 1; i <= n && OCI_STATUS; i++)
    {
        OCI_Elem *elem = CollGetElem(coll, i);

        quote = TRUE;

        if (ElemIsNull(elem))
        {
            len += StringAddToBuffer(str, len, OCI_STRING_NULL, OCI_STRING_NULL_SIZE, FALSE);
        }
        else
        {
            void *data = NULL;
            unsigned int data_size = 0;
            const unsigned int data_type = coll->typinf->cols[0].datatype;

            switch (data_type)
            {
                case OCI_CDT_TEXT:
                {
                    data_size = OCIStringSize(OCILib.env, elem->handle);
                    data      = (void *) ElemGetString(elem);
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
                    data  = (void *) ElemGetDate(elem);
                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    data  = (void *) ElemGetTimestamp(elem);
                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    data  = (void *) ElemGetInterval(elem);
                    break;
                }
                case OCI_CDT_LOB:
                {
                    data  = (void *) ElemGetLob(elem);
                    break;
                }
                case OCI_CDT_FILE:
                {
                    data  = (void *) ElemGetFile(elem);
                    break;
                }
                case OCI_CDT_REF:
                {
                    data  = (void *) ElemGetRef(elem);
                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    data  = (void *) ElemGetObject(elem);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    data  =  (void *) ElemGetColl(elem);
                    quote = FALSE;
                }
            }

            OCI_STATUS = (NULL != data || OCI_CDT_TEXT == data_type) && (NULL == err || !err->raise);

            if (OCI_STATUS)
            {
                otext *tmpbuf = str;

                if (tmpbuf)
                {
                    tmpbuf += len;
                }

                if (data)
                {
                    len += StringGetFromType(coll->con, &coll->typinf->cols[0], data, data_size, tmpbuf, tmpbuf && size ? *size - len : 0, quote);
                }
                else
                {
                    len += StringAddToBuffer(str, len, OCI_STRING_NULL, OCI_STRING_NULL_SIZE, FALSE);
                }
                OCI_STATUS = (NULL == err || OCI_UNKNOWN == err->type);
            }
        }

        if (OCI_STATUS && i < n)
        {
            len += StringAddToBuffer(str, len, OTEXT(", "), 2, FALSE);
        }
    }

    if (OCI_STATUS)
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

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}
