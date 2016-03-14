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

#define OCI_ELEM_SET_VALUE(elemtype, type, func_init, func_assign)                  \
                                                                                    \
    OCI_LIB_CALL_ENTER(boolean, FALSE)                                              \
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)                                            \
    OCI_CHECK_COMPAT(elem->con, elemtype == elem->typinf->cols[0].datatype)         \
                                                                                    \
    if (!value)                                                                     \
    {                                                                               \
        call_status = OCI_ElemSetNull(elem);                                        \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        if (!elem->obj)                                                             \
        {                                                                           \
            func_init;                                                              \
        }                                                                           \
                                                                                    \
        if (elem->obj)                                                              \
        {                                                                           \
            call_status = func_assign;                                              \
                                                                                    \
            if (call_status)                                                        \
            {                                                                       \
                OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);                    \
                elem->handle = ((type) elem->obj)->handle;                          \
            }                                                                       \
        }                                                                           \
    }                                                                               \
    call_retval = call_status;                                                      \
    OCI_LIB_CALL_EXIT()

#define OCI_ELEM_GET_VALUE(elemtype, type, func)                                    \
                                                                                    \
    OCI_LIB_CALL_ENTER(type, NULL)                                                  \
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)                                            \
    OCI_CHECK_COMPAT(elem->con, elemtype == elem->typinf->cols[0].datatype)         \
                                                                                    \
    call_status = TRUE;                                                             \
                                                                                    \
    if (OCI_IND_NULL != *elem->pind)                                                \
    {                                                                               \
        if (!elem->init)                                                            \
        {                                                                           \
            call_retval = func;                                                     \
                                                                                    \
            elem->init = (NULL != call_retval);                                     \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            call_retval = (type) elem->obj;                                         \
        }                                                                           \
                                                                                    \
        call_status = elem->init;                                                   \
    }                                                                               \
    OCI_LIB_CALL_EXIT()


#define OCI_ELEM_GET_NUMBER(elem, number_type, type, value)                         \
                                                                                    \
    OCI_LIB_CALL_ENTER(type, value)                                                 \
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)                                            \
    OCI_CHECK_COMPAT(elem->con, OCI_CDT_NUMERIC == elem->typinf->cols[0].datatype)  \
                                                                                    \
    call_status = OCI_ElemGetNumber(elem, (void *)&call_retval,                     \
                                    (uword) sizeof(call_retval),                    \
                                    (uword)number_type);                            \
                                                                                    \
    OCI_LIB_CALL_EXIT()

#define OCI_ELEM_SET_NUMBER(elem, value, number_type)                               \
                                                                                    \
    OCI_LIB_CALL_ENTER(boolean, FALSE)                                              \
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)                                            \
    OCI_CHECK_COMPAT(elem->con, OCI_CDT_NUMERIC == elem->typinf->cols[0].datatype)  \
                                                                                    \
    call_retval = call_status = OCI_ElemSetNumber(elem, (void *)&value,             \
                                                  (uword) sizeof(value),            \
                                                  (uword)number_type);              \
                                                                                    \
    OCI_LIB_CALL_EXIT()

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
* OCI_ElemFreeAllocatedData
* --------------------------------------------------------------------------------------------- */

void OCI_ElemFreeAllocatedData
(
    OCI_Elem *elem
)
{
    if (elem->alloc && elem->handle)
    {
        switch (elem->typinf->cols[0].datatype)
        {
        case OCI_CDT_TEXT:
            OCI_StringFreeStringPtr(elem->con->env, (OCIString **) &elem->handle, elem->con->err);
            break;
        default:
            OCI_FREE(elem->handle)
                break;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemInit
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * OCI_ElemInit
(
    OCI_Connection *con,
    OCI_Elem      **pelem,
    void           *handle,
    OCIInd         *pind,
    OCI_TypeInfo   *typinf
)
{
    OCI_Elem *elem = NULL;
    boolean   res  = FALSE;

    OCI_CHECK(NULL == pelem, NULL);

    if (!*pelem)
    {
        *pelem = (OCI_Elem *) OCI_MemAlloc(OCI_IPC_ELEMENT, sizeof(*elem), (size_t) 1, TRUE);
    }

    if (*pelem)
    {
        res = TRUE;

        elem = *pelem;
   
        elem->con = con;

        // Free previously allocated data before reassign new one
        OCI_ElemFreeAllocatedData(elem);

        // Reset members
        elem->ind    = OCI_IND_NULL;
        elem->typinf = typinf;
        elem->handle = handle;
        elem->init   = FALSE;
        elem->alloc  = FALSE;
        elem->hstate = handle ? OCI_OBJECT_FETCHED_CLEAN : OCI_OBJECT_ALLOCATED;
    
        switch (elem->typinf->cols[0].datatype)
        {
            case OCI_CDT_NUMERIC:
            {
                if (!elem->handle)
                {
                    elem->handle = (OCINumber * )OCI_MemAlloc(OCI_IPC_VOID, elem->typinf->cols[0].size, 1, TRUE);
                    elem->alloc  = TRUE;
                }
                break;
            }
            case OCI_CDT_BOOLEAN:
            {
                if (!elem->handle)
                {
                    elem->handle = (boolean*) OCI_MemAlloc(OCI_IPC_VOID, sizeof(boolean), 1, TRUE);
                    elem->alloc = TRUE;
                }
                break;
            }
            case OCI_CDT_TEXT:
            case OCI_CDT_TIMESTAMP:
            case OCI_CDT_INTERVAL:
            case OCI_CDT_RAW:
            case OCI_CDT_LOB:
            case OCI_CDT_FILE:
            case OCI_CDT_REF:
            {
                if (elem->handle)
                {
                    elem->handle = * (void **) handle;
                }
                break;
            }
        }

        if (pind)
        {
            elem->pind = pind;
            elem->ind  = *elem->pind;
        }
        else
        {
            elem->pind = &elem->ind;
        }
    }

    /* check for failure */

    if (!res)
    {
        OCI_ElemFree(elem);
        *pelem = elem = NULL;
    }

    return elem;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetNullIndicator
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ElemSetNullIndicator
(
    OCI_Elem *elem,
    OCIInd    value
)
{
    boolean res = TRUE;
    boolean set = FALSE;

    if (!elem->pind)
    {
        return FALSE;
    }

    if (OCI_CDT_OBJECT == elem->typinf->cols[0].datatype)
    {
        OCI_Object *obj = (OCI_Object*) elem->obj;

        if (obj)
        {
            elem->pind = obj->tab_ind;
            set = TRUE;            
        }
        else
        {
            value = OCI_IND_NULL;
        }

    }
    
    if (!set)
    {
        *elem->pind = value;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetNumber
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ElemSetNumber
(
    OCI_Elem *elem,
    void     *value,
    uword     size,
    uword     flag
)
{
     boolean res = TRUE;
 
     OCI_Column *col = &elem->typinf->cols[0];

    if (OCI_NUM_NUMBER != col->subtype)
    {
        /* for PL/SQL PLS_INTEGER and BINARY_INTEGER types, the values are not OCINumber but of scalar C int type !! */

        OCINumber tmp;
        memset(&tmp, 0, sizeof(tmp));

        res = res && OCI_NumberSet(elem->typinf->con, &tmp, col->size, col->subtype, col->libcode, value);
        res = res && OCI_NumberGet(elem->typinf->con, &tmp, size, flag, col->libcode, elem->handle);
    }
    else
    {      
        res = OCI_NumberSet(elem->con, (OCINumber *)elem->handle, size,  flag, col->libcode, value);
    }

    if (res)
    {
        OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetNumber
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ElemGetNumber
(
    OCI_Elem *elem,
    void     *value,
    uword     size,
    uword     flag
)
{
    boolean res = FALSE;

    OCI_CHECK(NULL == elem, FALSE)
    OCI_CHECK(NULL == value, FALSE)

    if (OCI_ElemIsNull(elem))
    {
        res = TRUE;
    }
    else if (OCI_CDT_NUMERIC == elem->typinf->cols[0].datatype)
    {
        OCI_Column *col = &elem->typinf->cols[0];

        res = TRUE;

        if (OCI_NUM_NUMBER != col->subtype)
        {
            /* for PL/SQL PLS_INTEGER and BINARY_INTEGER types, the values are not OCINumber but of scalar C int type !! */

            OCINumber tmp;
            memset(&tmp, 0, sizeof(tmp));

            res = res && OCI_NumberSet(elem->typinf->con, &tmp, col->size, col->subtype, col->libcode, elem->handle);
            res = res && OCI_NumberGet(elem->typinf->con, &tmp, size, flag, col->libcode, value);
        }
        else
        {
            OCINumber *num = (OCINumber *)elem->handle;

            res = OCI_NumberGet(elem->con, num, size, flag, col->libcode, value);
        }
    }
    else if (OCI_CDT_TEXT == elem->typinf->cols[0].datatype)
    {
        res = OCI_NumberFromString(elem->con, value, size, flag,
                                   elem->typinf->cols[0].libcode, 
                                   OCI_ElemGetString(elem), NULL);
    }
    else
    {
        OCI_ExceptionTypeNotCompatible(elem->con);
        res = FALSE;
    }

    return res;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * OCI_API OCI_ElemCreate
(
    OCI_TypeInfo *typinf
)
{
    OCI_LIB_CALL_ENTER(OCI_Elem*, NULL)

    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)

    call_retval = OCI_ElemInit(typinf->con, &call_retval, NULL, (OCIInd *)NULL, typinf);
    call_status = (NULL != call_retval);
  
    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemFree
(
    OCI_Elem *elem
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CHECK_OBJECT_FETCHED(elem)

    /* if the element has sub-objects that have been fetched, we need to free
       these objects */

    if (elem->obj)
    {
        OCI_Datatype * data = (OCI_Datatype *) elem->obj;

        if (OCI_OBJECT_FETCHED_CLEAN == data->hstate)
        {
            data->hstate = OCI_OBJECT_FETCHED_DIRTY;
        }

        OCI_FreeObjectFromType(elem->obj, elem->typinf->cols[0].datatype);
    }

    OCI_ElemFreeAllocatedData(elem);

    OCI_FREE(elem->tmpbuf)
    OCI_FREE(elem)

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetBoolean
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemGetBoolean
(
    OCI_Elem *elem
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CHECK_COMPAT(elem->con, OCI_CDT_BOOLEAN == elem->typinf->cols[0].datatype)

    call_status = TRUE;

    if (!OCI_ElemIsNull(elem))
    {
        boolean *data = (boolean *)elem->handle;

        if (data)
        {
            call_retval = *data;
        }
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetShort
 * --------------------------------------------------------------------------------------------- */

short OCI_API OCI_ElemGetShort
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_NUMBER(elem, OCI_NUM_SHORT, short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

unsigned short OCI_API OCI_ElemGetUnsignedShort
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_NUMBER(elem, OCI_NUM_USHORT, unsigned short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetInt
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ElemGetInt
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_NUMBER(elem, OCI_NUM_INT, int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ElemGetUnsignedInt
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_NUMBER(elem, OCI_NUM_UINT, unsigned int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetBigInt
 * --------------------------------------------------------------------------------------------- */

big_int OCI_API OCI_ElemGetBigInt
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_NUMBER(elem, OCI_NUM_BIGINT, big_int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_ElemGetUnsignedBigInt
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_NUMBER(elem, OCI_NUM_BIGUINT, big_uint, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetDouble
 * --------------------------------------------------------------------------------------------- */

double OCI_API OCI_ElemGetDouble
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_NUMBER(elem, OCI_NUM_DOUBLE, double, 0.0)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetFloat
 * --------------------------------------------------------------------------------------------- */

float OCI_API OCI_ElemGetFloat
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_NUMBER(elem, OCI_NUM_FLOAT, float, 0.0f)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_ElemGetString
(
    OCI_Elem *elem
)
{
    OCI_LIB_CALL_ENTER(otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CHECK_COMPAT(elem->con, OCI_CDT_TEXT == elem->typinf->cols[0].datatype)

    if (elem->handle)
    {
        call_retval = OCI_StringFromStringPtr(elem->con->env, (OCIString *) elem->handle, &elem->tmpbuf, &elem->tmpsize);
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetRaw
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ElemGetRaw
(
    OCI_Elem    *elem,
    void        *value,
    unsigned int len
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CHECK_COMPAT(elem->con, OCI_CDT_RAW == elem->typinf->cols[0].datatype)

    if (elem->handle)
    {
        OCIRaw *raw = (OCIRaw *) elem->handle; 
        ub4 raw_len = OCIRawSize(elem->con->env, raw);

        if (len > raw_len)
        {
            len = raw_len;
        }

        memcpy(value, OCIRawPtr(elem->con->env, raw), (size_t) len);
        
        call_retval = TRUE;
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* -------------------------------------------------------------------------------------------- *
* OCI_ElemGetRawSize
* --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ElemGetRawSize
(
    OCI_Elem    *elem
)
{
    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CHECK_COMPAT(elem->con, OCI_CDT_RAW == elem->typinf->cols[0].datatype)

    if (elem->handle)
    {
        OCIRaw *raw = (OCIRaw *)elem->handle;
        call_retval = OCIRawSize(elem->con->env, raw);
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OCI_API OCI_ElemGetDate
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_DATETIME,
        OCI_Date *,
        OCI_DateInit(elem->con, (OCI_Date **) &elem->obj, (OCIDate *) elem->handle, FALSE, FALSE)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_API OCI_ElemGetTimestamp
(
    OCI_Elem *elem
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_TIMESTAMP,
        OCI_Timestamp *,
        OCI_TimestampInit(elem->con, (OCI_Timestamp **) &elem->obj, 
                          (OCIDateTime *) elem->handle,
                          elem->typinf->cols[0].subtype)
    )

#else

    OCI_LIB_CALL_ENTER( OCI_Timestamp *, NULL)
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    call_status = TRUE;
    OCI_LIB_CALL_EXIT()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OCI_API OCI_ElemGetInterval
(
    OCI_Elem *elem
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_INTERVAL,
        OCI_Interval *,
        OCI_IntervalInit(elem->con, (OCI_Interval **) &elem->obj, 
                         (OCIInterval *) elem->handle,
                         elem->typinf->cols[0].subtype)
    )
#else

    OCI_LIB_CALL_ENTER( OCI_Interval *, NULL)
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    call_status = TRUE;
    OCI_LIB_CALL_EXIT()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OCI_API OCI_ElemGetLob
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_LOB, 
        OCI_Lob*,
        OCI_LobInit(elem->con, (OCI_Lob **) &elem->obj, 
                    (OCILobLocator *) elem->handle,
                    elem->typinf->cols[0].subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * OCI_API OCI_ElemGetFile
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_FILE,
        OCI_File*,
        OCI_FileInit(elem->con, (OCI_File **) &elem->obj, 
                     (OCILobLocator *) elem->handle,
                     elem->typinf->cols[0].subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetRef
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OCI_API OCI_ElemGetRef
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_REF,
        OCI_Ref*,
        OCI_RefInit(elem->con, &elem->typinf->cols[0].typinf,
                    (OCI_Ref **) &elem->obj,  (OCIRef *) elem->handle)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OCI_API OCI_ElemGetObject
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_OBJECT,
        OCI_Object*,
        OCI_ObjectInit(elem->con, (OCI_Object **) &elem->obj, elem->handle,
                       elem->typinf->cols[0].typinf, NULL, -1, TRUE)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetColl
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_API OCI_ElemGetColl
(
    OCI_Elem *elem
)
{
    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCI_Coll*,
        OCI_CollInit(elem->con, (OCI_Coll **) &elem->obj,  (OCIColl *) elem->handle,
                     elem->typinf->cols[0].typinf)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetBoolean
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetBoolean
(
    OCI_Elem *elem,
    boolean   value
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CHECK_COMPAT(elem->con, OCI_CDT_BOOLEAN == elem->typinf->cols[0].datatype)
     
    if (elem->handle)
    {
 		boolean *data = (boolean *) elem->handle;
        call_status = TRUE;
 
		*data = value;
     
        OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetShort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetShort
(
    OCI_Elem *elem,
    short     value
)
{
    OCI_ELEM_SET_NUMBER(elem,value, OCI_NUM_SHORT)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetUnsignedShort
(
    OCI_Elem      *elem,
    unsigned short value
)
{
    OCI_ELEM_SET_NUMBER(elem, value, OCI_NUM_USHORT)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetInt
(
    OCI_Elem *elem,
    int       value
)
{
    OCI_ELEM_SET_NUMBER(elem, value, OCI_NUM_INT)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetUnsignedInt
(
    OCI_Elem    *elem,
    unsigned int value
)
{
    OCI_ELEM_SET_NUMBER(elem, value, OCI_NUM_UINT)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetBigInt
(
    OCI_Elem *elem,
    big_int   value
)
{
    OCI_ELEM_SET_NUMBER(elem, value, OCI_NUM_BIGINT)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetUnsignedBigInt
(
    OCI_Elem *elem,
    big_uint  value
)
{
    OCI_ELEM_SET_NUMBER(elem, value, OCI_NUM_BIGUINT)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetDouble
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetDouble
(
    OCI_Elem *elem,
    double    value
)
{
    OCI_ELEM_SET_NUMBER(elem, value, OCI_NUM_DOUBLE)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetFloat
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetFloat
(
    OCI_Elem *elem,
    float     value
)
{
    OCI_ELEM_SET_NUMBER(elem, value, OCI_NUM_FLOAT)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetString
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetString
(
    OCI_Elem    *elem,
    const otext *value
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CHECK_COMPAT(elem->con, OCI_CDT_TEXT == elem->typinf->cols[0].datatype)

    if (!value)
    {
        call_status = OCI_ElemSetNull(elem);
    }
    else
    {
        elem->alloc = (elem->alloc || elem->handle == NULL);

        call_status = OCI_StringToStringPtr(elem->con->env,  (OCIString **) &elem->handle,
                                            elem->con->err, value);

        if (call_status)
        {
            OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);
        }
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetRaw
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetRaw
(
    OCI_Elem    *elem,
    void       * value,
    unsigned int len
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    OCI_CHECK_COMPAT(elem->con, OCI_CDT_TEXT == elem->typinf->cols[0].datatype)

    if (!value)
    {
        call_status = OCI_ElemSetNull(elem);
    }
    else
    {
        call_status = TRUE;

        OCI_CALL2
        (
            call_status, elem->con,

            OCIRawAssignBytes(elem->con->env, elem->con->err, (ub1*) value,
                              len, (OCIRaw **) &elem->handle)
        )

        if (call_status)
        {
            OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);
        }
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetDate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetDate
(
    OCI_Elem *elem,
    OCI_Date *value
)
{
    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_DATETIME,
        OCI_Date*,
        OCI_DateInit(elem->con, (OCI_Date **) &elem->obj, (OCIDate *) elem->handle, TRUE, FALSE),
        OCI_DateAssign((OCI_Date *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetTimestamp
(
    OCI_Elem      *elem,
    OCI_Timestamp *value
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_TIMESTAMP, 
        OCI_Timestamp*,
        OCI_TimestampInit(elem->con, (OCI_Timestamp **) &elem->obj,
                          (OCIDateTime *) elem->handle,  elem->typinf->cols[0].subtype),
        OCI_TimestampAssign((OCI_Timestamp *) elem->obj, value)
    )

#else

    OCI_LIB_CALL_ENTER(boolean, FALSE)
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    call_status = TRUE;
    OCI_LIB_CALL_EXIT()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetInterval
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetInterval
(
    OCI_Elem     *elem,
    OCI_Interval *value
)
{

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_ELEM_SET_VALUE
    ( 
        OCI_CDT_INTERVAL,
        OCI_Interval*,
        OCI_IntervalInit(elem->con, (OCI_Interval **) &elem->obj,
                         (OCIInterval *) elem->handle, elem->typinf->cols[0].subtype),
        OCI_IntervalAssign((OCI_Interval *) elem->obj, value)
    )

#else

    OCI_LIB_CALL_ENTER(boolean, FALSE)
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    call_status = TRUE;
    OCI_LIB_CALL_EXIT()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetColl
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetColl
(
    OCI_Elem *elem,
    OCI_Coll *value
)
{
    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCI_Coll*,
        OCI_CollInit(elem->con, (OCI_Coll **) &elem->obj,
                         (OCIColl *) elem->handle, elem->typinf->cols[0].typinf),
        OCI_CollAssign((OCI_Coll *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetObject
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetObject
(
    OCI_Elem   *elem,
    OCI_Object *value
)
{
    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_OBJECT,
        OCI_Object*,
        OCI_ObjectInit(elem->con, (OCI_Object **) &elem->obj, elem->handle, 
                       elem->typinf->cols[0].typinf, NULL, -1, TRUE),
        OCI_ObjectAssign((OCI_Object *) elem->obj, value)
    )
 }

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetLob
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetLob
(
    OCI_Elem *elem,
    OCI_Lob  *value
)
{
    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_LOB,
        OCI_Lob*,
        OCI_LobInit(elem->con, (OCI_Lob **) &elem->obj, (OCILobLocator *) elem->handle,
                    elem->typinf->cols[0].subtype),
        OCI_LobAssign((OCI_Lob *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetFile
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetFile
(
    OCI_Elem *elem,
    OCI_File *value
)
{
    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_FILE,
        OCI_File*,
        OCI_FileInit(elem->con, (OCI_File **) &elem->obj, (OCILobLocator *) elem->handle,
                     elem->typinf->cols[0].subtype),
        OCI_FileAssign((OCI_File *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetRef
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetRef
(
    OCI_Elem *elem,
    OCI_Ref  *value
)
{
    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_REF,
        OCI_Ref*,
        OCI_RefInit(elem->con, &elem->typinf->cols[0].typinf, (OCI_Ref **) &elem->obj,
                    (OCIRef *) elem->handle),
        OCI_RefAssign((OCI_Ref *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemIsNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemIsNull
(
    OCI_Elem *elem
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)

    call_status = TRUE;

    if (elem->pind)
    {
        call_status = (OCI_IND_NULL == *elem->pind);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetNull
(
    OCI_Elem *elem
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem)

    call_retval = call_status = OCI_ElemSetNullIndicator(elem, OCI_IND_NULL);

    OCI_LIB_CALL_EXIT()
}
