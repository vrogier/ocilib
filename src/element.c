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
 * $Id: element.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"


#define OCI_ELEM_SET_VALUE(elemtype, type, func_init, func_assign)                  \
                                                                                    \
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);                                    \
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].datatype == elemtype, FALSE); \
                                                                                    \
    if (!value)                                                                     \
    {                                                                               \
        res = OCI_ElemSetNull(elem);                                                \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        if (!elem->obj)                                                             \
        {                                                                           \
            func_init;                                                              \
        }                                                                           \
                                                                                    \
        if (!elem->obj)                                                             \
        {                                                                           \
            res = func_assign;                                                      \
                                                                                    \
            if (res)                                                                \
            {                                                                       \
                OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);                    \
                elem->handle = ((type *) elem->obj)->handle;                        \
            }                                                                       \
        }                                                                           \
    }                                                                               \
                                                                                    \

#define OCI_ELEM_GET_VALUE(elemtype, ret, type, func)                               \
                                                                                    \
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);                                    \
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].datatype == elemtype, FALSE); \
                                                                                    \
    if (OCI_IND_NULL != elem->ind)                                                  \
    {                                                                               \
        if (!elem->init)                                                            \
        {                                                                           \
            ret = func;                                                             \
                                                                                    \
            elem->init = (ret != NULL);                                             \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            ret = (type *) elem->obj;                                               \
        }                                                                           \
                                                                                    \
        res = elem->init;                                                           \
    }                                                                               \

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

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
    boolean res    = TRUE;

    OCI_CHECK(pelem == NULL, NULL);

    if (!*pelem)
    {
        *pelem = (OCI_Elem *) OCI_MemAlloc(OCI_IPC_ELEMENT, sizeof(*elem), (size_t) 1, TRUE);
    }

    if (*pelem)
    {
        elem = *pelem;

        elem->con    = con;
        elem->ind    = OCI_IND_NULL;
        elem->typinf = typinf;
        elem->handle = handle;
        elem->init   = FALSE;
        elem->hstate = handle ? OCI_OBJECT_FETCHED_CLEAN : OCI_OBJECT_ALLOCATED;
                
        switch (elem->typinf->cols[0].datatype)
        {
            case OCI_CDT_NUMERIC:
            {
                if (!elem->handle)
                {
                    elem->handle = (OCINumber *) OCI_MemAlloc(OCI_IPC_VOID, sizeof(OCINumber), 1, TRUE);
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
    else
    {
        res = FALSE;
    }

    /* check for failure */

    if (!res)
    {
        OCI_ElemFree(elem);
        elem = NULL;
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

    if (elem->pind)
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
    boolean res = FALSE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].datatype == OCI_CDT_NUMERIC, FALSE);

    res = OCI_NumberSet(elem->con, (OCINumber *) elem->handle, size, flag, elem->typinf->cols[0].libcode, value);

    OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);

    OCI_RESULT(res);

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);

    if (OCI_CDT_NUMERIC == elem->typinf->cols[0].datatype)
    {
        OCINumber *num = (OCINumber *) elem->handle;

        res = OCI_NumberGet(elem->con, num, size, flag, elem->typinf->cols[0].libcode, value);
    }
    else if (OCI_CDT_TEXT == elem->typinf->cols[0].datatype)
    {
        res = OCI_NumberFromString(elem->con, value, size, flag, elem->typinf->cols[0].libcode, OCI_ElemGetString(elem), NULL);
    }
    else
    {
        OCI_ExceptionTypeNotCompatible(elem->con);
    }

    OCI_RESULT(res);

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
    OCI_Elem *elem = NULL;

    OCI_CHECK_INITIALIZED(NULL);
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);

    elem = OCI_ElemInit(typinf->con, &elem, NULL, (OCIInd *) NULL, typinf);

    OCI_RESULT(elem != NULL);

    return elem;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemFree
(
    OCI_Elem *elem
)
{
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);

    OCI_CHECK_OBJECT_FETCHED(elem, FALSE);

    /* if the element has sub-objects that have been fetched, we need to free
       these objects */

    if (elem->obj)
    {
        OCI_Datatype * data = (OCI_Datatype *) elem->obj;

        if (OCI_OBJECT_FETCHED_CLEAN == data->hstate)
        {
            data->hstate = OCI_OBJECT_FETCHED_DIRTY;
        }

        switch (elem->typinf->cols[0].datatype)
        {
            case OCI_CDT_DATETIME:
            {
                OCI_DateFree((OCI_Date *) elem->obj);
                break;
            }
            case OCI_CDT_LOB:
            {
                OCI_LobFree((OCI_Lob *) elem->obj);
                break;
            }
            case OCI_CDT_FILE:
            {
                OCI_FileFree((OCI_File *) elem->obj);
                break;
            }
            case OCI_CDT_OBJECT:
            {
                OCI_ObjectFree((OCI_Object *) elem->obj);
                break;
            }
            case OCI_CDT_COLLECTION:
            {
                OCI_CollFree((OCI_Coll *) elem->obj);
                break;
            }
            case OCI_CDT_TIMESTAMP:
            {
                OCI_TimestampFree((OCI_Timestamp *) elem->obj);
                break;
            }
            case OCI_CDT_INTERVAL:
            {
                OCI_IntervalFree((OCI_Interval *) elem->obj);
                break;
            }
            case OCI_CDT_REF:
            {
                OCI_RefFree((OCI_Ref *) elem->obj);
                break;
            }
        }
    }

    if ((OCI_OBJECT_ALLOCATED == elem->hstate) && (OCI_CDT_NUMERIC == elem->typinf->cols[0].datatype))
    {
        OCI_FREE(elem->handle);
    }

    OCI_FREE(elem->tmpbuf);
    OCI_FREE(elem);

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetShort
 * --------------------------------------------------------------------------------------------- */

short OCI_API OCI_ElemGetShort
(
    OCI_Elem *elem
)
{
    short value = 0;

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_SHORT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

unsigned short OCI_API OCI_ElemGetUnsignedShort
(
    OCI_Elem *elem
)
{
    unsigned short value = 0;

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_USHORT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetInt
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ElemGetInt
(
    OCI_Elem *elem
)
{
    int value = 0;

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_INT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ElemGetUnsignedInt
(
    OCI_Elem *elem
)
{
    unsigned int value = 0;

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_UINT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetBigInt
 * --------------------------------------------------------------------------------------------- */

big_int OCI_API OCI_ElemGetBigInt
(
    OCI_Elem *elem
)
{
    big_int value = 0;

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_BIGINT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_ElemGetUnsignedBigInt
(
    OCI_Elem *elem
)
{
    big_uint value = 0;

    OCI_ElemGetNumber(elem, (void *) &value,  (uword) sizeof(value), (uword) OCI_NUM_BIGUINT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetDouble
 * --------------------------------------------------------------------------------------------- */

double OCI_API OCI_ElemGetDouble
(
    OCI_Elem *elem
)
{
    double value = 0.0;

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_DOUBLE);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetFloat
 * --------------------------------------------------------------------------------------------- */

float OCI_API OCI_ElemGetFloat
(
    OCI_Elem *elem
)
{
    float value = 0.0f;

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_FLOAT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_ElemGetString
(
    OCI_Elem *elem
)
{
    const otext *str = NULL;
    boolean res      = FALSE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].datatype == OCI_CDT_TEXT, NULL);

    if (elem->handle)
    {
        res = TRUE;

        str = OCI_StringFromStringPtr(elem->con->env, (OCIString *) elem->handle, &elem->tmpbuf, &elem->tmpsize);
    }

    OCI_RESULT(res);

    return str;
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
    boolean res = FALSE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, 0);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].datatype == OCI_CDT_RAW, 0);

    if (elem->handle)
    {
        OCIRaw *raw = (OCIRaw *) elem->handle; 
        ub4 raw_len = OCIRawSize(elem->con->env, raw);

        if (len > raw_len)
        {
            len = raw_len;
        }

        memcpy(value, OCIRawPtr(elem->con->env, raw), (size_t) len);
        
        res = TRUE;
    }

    OCI_RESULT(res);

    return len;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OCI_API OCI_ElemGetDate
(
    OCI_Elem *elem
)
{
    boolean res    = TRUE;
    OCI_Date *date = NULL;

    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_DATETIME,
        date, 
        OCI_Date,
        OCI_DateInit(elem->con, (OCI_Date **) &elem->obj, (OCIDate *) elem->handle, FALSE, FALSE)
    )

    OCI_RESULT(res);

    return date;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_API OCI_ElemGetTimestamp
(
    OCI_Elem *elem
)
{
    boolean res         = TRUE;
    OCI_Timestamp *tmsp = NULL;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_TIMESTAMP,
        tmsp, 
        OCI_Timestamp,
        OCI_TimestampInit(elem->con, (OCI_Timestamp **) &elem->obj,  (OCIDateTime *) elem->handle,
                          elem->typinf->cols[0].subtype)
    )

#endif

    OCI_RESULT(res);

    return tmsp;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OCI_API OCI_ElemGetInterval
(
    OCI_Elem *elem
)
{
    boolean res       = TRUE;
    OCI_Interval *itv = NULL;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_INTERVAL,
        itv, 
        OCI_Interval,
        OCI_IntervalInit(elem->con, (OCI_Interval **) &elem->obj, (OCIInterval *) elem->handle,
                         elem->typinf->cols[0].subtype)
    )

#endif

    OCI_RESULT(res);

    return itv;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OCI_API OCI_ElemGetLob
(
    OCI_Elem *elem
)
{
    boolean res  = TRUE;
    OCI_Lob *lob = NULL;

    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_LOB,
        lob, 
        OCI_Lob,
        OCI_LobInit(elem->con, (OCI_Lob **) &elem->obj, (OCILobLocator *) elem->handle,
                    elem->typinf->cols[0].subtype)
    )

    OCI_RESULT(res);

    return lob;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * OCI_API OCI_ElemGetFile
(
    OCI_Elem *elem
)
{
    boolean res    = TRUE;
    OCI_File *file = NULL;

    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_FILE,
        file, 
        OCI_File,
        OCI_FileInit(elem->con, (OCI_File **) &elem->obj, (OCILobLocator *) elem->handle,
                     elem->typinf->cols[0].subtype)
    )


    OCI_RESULT(res);

    return file;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetRef
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OCI_API OCI_ElemGetRef
(
    OCI_Elem *elem
)
{
    boolean res  = TRUE;
    OCI_Ref *ref = NULL;

    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_REF,
        ref, 
        OCI_Ref,
        OCI_RefInit(elem->con, elem->typinf->cols[0].typinf,
                    (OCI_Ref **) &elem->obj,  (OCIRef *) elem->handle)
    )

    OCI_RESULT(res);

    return ref;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OCI_API OCI_ElemGetObject
(
    OCI_Elem *elem
)
{
    boolean res     = TRUE;
    OCI_Object *obj = NULL;

    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_OBJECT,
        obj, 
        OCI_Object,
        OCI_ObjectInit(elem->con, (OCI_Object **) &elem->obj, elem->handle,
                       elem->typinf->cols[0].typinf, NULL, -1, TRUE)
    )

    OCI_RESULT(res);

    return obj;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetColl
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_API OCI_ElemGetColl
(
    OCI_Elem *elem
)
{
    boolean res    = TRUE;
    OCI_Coll *coll = NULL;

    OCI_ELEM_GET_VALUE
    (
        OCI_CDT_COLLECTION,
        coll, 
        OCI_Coll,
        OCI_CollInit(elem->con, (OCI_Coll **) &elem->obj,  (OCIColl *) elem->handle,
                     elem->typinf->cols[0].typinf)
    )


    OCI_RESULT(res);

    return coll;
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_SHORT);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_USHORT);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_INT);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_UINT);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_BIGINT);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_BIGUINT);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_DOUBLE);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), (uword) OCI_NUM_FLOAT);
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].datatype == OCI_CDT_TEXT, FALSE);

    if (!value)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        res = OCI_StringToStringPtr(elem->con->env,
                                    (OCIString **) &elem->handle,
                                    elem->con->err, value);

        OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);
    }

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].datatype == OCI_CDT_RAW, FALSE);

    if (!value)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        OCI_CHECK_MIN(elem->con, NULL, len, 1, FALSE);

        OCI_CALL2
        (
            res, elem->con,

            OCIRawAssignBytes(elem->con->env, elem->con->err, (ub1*) value,
                              len, (OCIRaw **) &elem->handle)
        )

        OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);
    }

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_DATETIME,
        OCI_Date,
        OCI_DateInit(elem->con, (OCI_Date **) &elem->obj, (OCIDate *) elem->handle, TRUE, FALSE),
        OCI_DateAssign((OCI_Date *) elem->obj, value)
    )

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_TIMESTAMP, 
        OCI_Timestamp,
        OCI_TimestampInit(elem->con, (OCI_Timestamp **) &elem->obj,
                          (OCIDateTime *) elem->handle,  elem->typinf->cols[0].subtype),
        OCI_TimestampAssign((OCI_Timestamp *) elem->obj, value)
    )

#endif

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_ELEM_SET_VALUE
    ( 
        OCI_CDT_INTERVAL,
        OCI_Interval,
        OCI_IntervalInit(elem->con, (OCI_Interval **) &elem->obj,
                         (OCIInterval *) elem->handle, elem->typinf->cols[0].subtype),
        OCI_IntervalAssign((OCI_Interval *) elem->obj, value)
    )

#endif

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCI_Coll,
        OCI_CollInit(elem->con, (OCI_Coll **) &elem->obj,
                         (OCIColl *) elem->handle, elem->typinf->cols[0].typinf),
        OCI_CollAssign((OCI_Coll *) elem->obj, value)
    )

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_OBJECT,
        OCI_Object,
        OCI_ObjectInit(elem->con, (OCI_Object **) &elem->obj, elem->handle, 
                       elem->typinf->cols[0].typinf, NULL, -1, TRUE),
        OCI_ObjectAssign((OCI_Object *) elem->obj, value)
    )
    
    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;
    
    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_LOB,
        OCI_Lob,
        OCI_LobInit(elem->con, (OCI_Lob **) &elem->obj, (OCILobLocator *) elem->handle,
                    elem->typinf->cols[0].subtype),
        OCI_LobAssign((OCI_Lob *) elem->obj, value)
    )
    
    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_FILE,
        OCI_File,
        OCI_FileInit(elem->con, (OCI_File **) &elem->obj, (OCILobLocator *) elem->handle,
                     elem->typinf->cols[0].subtype),
        OCI_FileAssign((OCI_File *) elem->obj, value)
    )
    
    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_ELEM_SET_VALUE
    (
        OCI_CDT_REF,
        OCI_Ref,
        OCI_RefInit(elem->con,elem->typinf->cols[0].typinf, (OCI_Ref **) &elem->obj,
                    (OCIRef *) elem->handle),
        OCI_RefAssign((OCI_Ref *) elem->obj, value)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemIsNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemIsNull
(
    OCI_Elem *elem
)
{
    boolean ret = FALSE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);

    if (elem->pind)
    {
        ret = (*elem->pind == OCI_IND_NULL);
    }

    OCI_RESULT(TRUE);

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetNull
(
    OCI_Elem *elem
)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);

    res = OCI_ElemSetNullIndicator(elem, OCI_IND_NULL);

    OCI_RESULT(res);

    return res;
}
