/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2010 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: element.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

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

    if (*pelem == NULL)
        *pelem = (OCI_Elem *) OCI_MemAlloc(OCI_IPC_ELEMENT, sizeof(*elem),
                                           (size_t) 1, TRUE);

    if (*pelem != NULL)
    {
        elem = *pelem;

        elem->con    = con;
        elem->ind    = OCI_IND_NULL;
        elem->typinf = typinf;
        elem->handle = handle;
        elem->init   = FALSE;

        if (handle == NULL)
            elem->hstate = OCI_OBJECT_ALLOCATED;
        else
            elem->hstate = OCI_OBJECT_FETCHED_CLEAN;

        switch (elem->typinf->cols[0].type)
        {
            case OCI_CDT_NUMERIC:

                if (elem->handle == NULL)
                {
                    elem->handle = (OCINumber *) OCI_MemAlloc(OCI_IPC_VOID,
                                                              sizeof(OCINumber),
                                                              1, TRUE);
                }

                break;

            case OCI_CDT_TEXT:
            case OCI_CDT_TIMESTAMP:
            case OCI_CDT_INTERVAL:
            case OCI_CDT_RAW:
            case OCI_CDT_LOB:
            case OCI_CDT_FILE:
            case OCI_CDT_REF:

                if (elem->handle != NULL)
                    elem->handle = * (void **) handle;
                break;
        }

        if (pind != NULL)
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
        res = FALSE;

    /* check for failure */

    if (res == FALSE)
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

    if (elem->typinf->cols[0].type == OCI_CDT_OBJECT)
    {
        OCI_Object *obj = (OCI_Object *) elem->obj;

        if (obj != NULL)
        {
            elem->pind = obj->tab_ind;
        }
    }
    else
    {
        if (elem->pind != NULL)
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
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_NUMERIC, FALSE);

    res = OCI_NumberSet(elem->con, (OCINumber *) elem->handle, value, size, flag);

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

    if (elem->typinf->cols[0].type == OCI_CDT_NUMERIC)
    {
        OCINumber *num = (OCINumber *) elem->handle;

        res = OCI_NumberGet(elem->con, num, value, size, flag);
    }
    else if (elem->typinf->cols[0].type == OCI_CDT_TEXT)
    {
        const mtext *fmt = OCI_GetDefaultFormatNumeric(elem->con);
        ub4 fmt_size     = (ub4) mtslen(fmt);
        dtext *data      = (dtext *) OCI_ElemGetString(elem);

        res = OCI_NumberGetFromStr(elem->con, value, size, flag, data,
                                   (int) dtslen(data),  fmt, fmt_size);
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

    if (elem->obj != NULL)
    {
        OCI_Datatype * data = (OCI_Datatype *) elem->obj;

        if (data->hstate == OCI_OBJECT_FETCHED_CLEAN)
            data->hstate = OCI_OBJECT_FETCHED_DIRTY;

        switch (elem->typinf->cols[0].type)
        {
            case OCI_CDT_DATETIME:

                OCI_DateFree((OCI_Date *) elem->obj);
                break;

            case OCI_CDT_LOB:

                OCI_LobFree((OCI_Lob *) elem->obj);
                break;

            case OCI_CDT_FILE:

                OCI_FileFree((OCI_File *) elem->obj);
                break;

            case OCI_CDT_OBJECT:

                OCI_ObjectFree((OCI_Object *) elem->obj);
                break;

            case OCI_CDT_COLLECTION:

                OCI_CollFree((OCI_Coll *) elem->obj);;
                break;

            case OCI_CDT_TIMESTAMP:

                OCI_TimestampFree((OCI_Timestamp *) elem->obj);
                break;

            case OCI_CDT_INTERVAL:

                OCI_IntervalFree((OCI_Interval *) elem->obj);
                break;

            case OCI_CDT_REF:

                OCI_RefFree((OCI_Ref *) elem->obj);
                break;
        }
    }

    if ((elem->hstate == OCI_OBJECT_ALLOCATED) &&
        (elem->typinf->cols[0].type == OCI_CDT_NUMERIC))
    {
        OCI_FREE(elem->handle);
    }

    OCI_FREE(elem->buf);
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

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(short),
                      (uword) OCI_NUM_SHORT);

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

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(unsigned short),
                      (uword) OCI_NUM_USHORT);

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

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(value),
                      (uword) OCI_NUM_INT);

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

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(value),
                      (uword) OCI_NUM_UINT);

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

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(big_int),
                      (uword) OCI_NUM_BIGINT);

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

    OCI_ElemGetNumber(elem, (void *) &value,  (uword) sizeof(big_uint),
                      (uword) OCI_NUM_BIGUINT);

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

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(double),
                      (uword) OCI_NUM_DOUBLE);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemGetString
 * --------------------------------------------------------------------------------------------- */

const dtext * OCI_API OCI_ElemGetString
(
    OCI_Elem *elem
)
{
    const dtext *str = NULL;
    boolean res      = FALSE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_TEXT, NULL);

    if (elem->handle != NULL)
    {
        res = TRUE;

        str = (dtext *) OCI_StringFromStringPtr((OCIString *) elem->handle,
                                                &elem->buf, &elem->buflen);
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
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_RAW, 0);

    if (elem->handle != NULL)
    {
        OCIRaw *raw = *(OCIRaw **) elem->handle;
        ub4 raw_len = 0;

        raw_len = OCIRawSize(OCILib.env, raw);

        if (len > raw_len)
            len = raw_len;

        memcpy(value, OCIRawPtr(OCILib.env, raw), (size_t) len);
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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_DATETIME, NULL);

    if (elem->ind != OCI_IND_NULL)
    {
        if (elem->init == FALSE)
        {
            date = OCI_DateInit(elem->con, (OCI_Date **) &elem->obj,
                                (OCIDate *) elem->handle, FALSE, FALSE);

            elem->init = (date != NULL);
        }
        else
            date = (OCI_Date *) elem->obj;

        res = elem->init;
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_TIMESTAMP, NULL);

    if (elem->ind != OCI_IND_NULL)
    {
        if (elem->init == FALSE)
        {
            tmsp = OCI_TimestampInit(elem->con, (OCI_Timestamp **) &elem->obj,
                                     (OCIDateTime *) elem->handle,
                                     elem->typinf->cols[0].subtype);

            elem->init = (tmsp != NULL);
        }
        else
            tmsp = (OCI_Timestamp *) elem->obj;

        res = elem->init;
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_INTERVAL, NULL);

    if (elem->ind != OCI_IND_NULL)
    {
        if (elem->init == FALSE)
        {
            itv = OCI_IntervalInit(elem->con, (OCI_Interval **) &elem->obj,
                                   (OCIInterval *) elem->handle,
                                   elem->typinf->cols[0].subtype);

            elem->init = (itv != NULL);
        }
        else
            itv = (OCI_Interval *) elem->obj;

        res = elem->init;
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_LOB, NULL);

    if (elem->ind != OCI_IND_NULL)
    {
        if (elem->init == FALSE)
        {
            lob = OCI_LobInit(elem->con, (OCI_Lob **) &elem->obj,
                              (OCILobLocator *) elem->handle,
                              elem->typinf->cols[0].subtype);

            elem->init = (lob != NULL);
        }
        else
            lob = (OCI_Lob *) elem->obj;

        res = elem->init;
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_FILE, NULL);

    if (elem->ind != OCI_IND_NULL)
    {
        if (elem->init == FALSE)
        {
            file = OCI_FileInit(elem->con, (OCI_File **) &elem->obj,
                                (OCILobLocator *) elem->handle,
                                elem->typinf->cols[0].subtype);

            elem->init = (file != NULL);
        }
        else
            file = (OCI_File *) elem->obj;

        res = elem->init;
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_REF, NULL);

    if (elem->ind != OCI_IND_NULL)
    {
        if (elem->init == FALSE)
        {
            ref = OCI_RefInit(elem->con, elem->typinf->cols[0].typinf,
                              (OCI_Ref **) &elem->obj,
                              (OCIRef *) elem->handle);

            elem->init = (ref != NULL);
        }
        else
            ref = (OCI_Ref *) elem->obj;

        res = elem->init;
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_OBJECT, NULL);

    if (elem->ind != OCI_IND_NULL)
    {
        if (elem->init == FALSE)
        {
            obj = OCI_ObjectInit(elem->con, (OCI_Object **) &elem->obj,
                                 elem->handle,
                                 elem->typinf->cols[0].typinf,
                                 NULL, -1, TRUE);

            elem->init = (obj != NULL);
        }
        else
            obj = (OCI_Object *) elem->obj;

        res = elem->init;
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_COLLECTION, NULL);

    if (elem->ind != OCI_IND_NULL)
    {
        if (elem->init == FALSE)
        {
            coll = OCI_CollInit(elem->con, (OCI_Coll **) &elem->obj,
                                (OCIColl *) elem->handle,
                                elem->typinf->cols[0].typinf);

            elem->init = (coll != NULL);
        }
        else
            coll = (OCI_Coll *) elem->obj;

        res = elem->init;
    }

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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value),
                             (uword) OCI_NUM_SHORT);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value),
                             (uword) OCI_NUM_USHORT);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value),
                             (uword) OCI_NUM_INT);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value),
                             (uword) OCI_NUM_UINT);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value),
                             (uword) OCI_NUM_BIGINT);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value),
                             (uword) OCI_NUM_BIGUINT);
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
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value),
                             (uword) OCI_NUM_DOUBLE);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ElemSetString
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ElemSetString
(
    OCI_Elem    *elem,
    const dtext *value
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_TEXT, FALSE);

    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        res = OCI_StringToStringPtr((OCIString **) &elem->handle,
                                    elem->con->err, (void *) value,
                                    &elem->buf, &elem->buflen);

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
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_RAW, FALSE);

    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        OCI_CHECK_MIN(elem->con, NULL, len, 1, FALSE);

        OCI_CALL2
        (
            res, elem->con,

            OCIRawAssignBytes(OCILib.env, elem->con->err, (ub1*) value,
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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_DATETIME, FALSE);

    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        if (elem->obj == NULL)
        {
            OCI_DateInit(elem->con, (OCI_Date **) &elem->obj,
                         (OCIDate *) elem->handle, TRUE, FALSE);
        }

        if (elem->obj != NULL)
        {
            res = OCI_DateAssign((OCI_Date *) elem->obj, value);

            if (res == TRUE)
            {
                OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);

                elem->handle = ((OCI_Date *) elem->obj)->handle;
            }
        }
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_TIMESTAMP, FALSE);

    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        if (elem->obj == NULL)
        {
            OCI_TimestampInit(elem->con, (OCI_Timestamp **) &elem->obj,
                              (OCIDateTime   *) elem->handle,
                              elem->typinf->cols[0].subtype);
        }

        if (elem->obj != NULL)
        {
            res = OCI_TimestampAssign((OCI_Timestamp *) elem->obj, value);

            if (res == TRUE)
            {
                OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);

                elem->handle = ((OCI_Timestamp *) elem->obj)->handle;
            }
        }
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_INTERVAL, FALSE);

    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        if (elem->obj == NULL)
        {
            OCI_IntervalInit(elem->con, (OCI_Interval **) &elem->obj,
                             (OCIInterval  *) elem->handle,
                             elem->typinf->cols[0].subtype);
        }

        if (elem->obj != NULL)
        {
            res = OCI_IntervalAssign((OCI_Interval *) elem->obj, value);

            if (res == TRUE)
            {
                OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);

                elem->handle = ((OCI_Interval *) elem->obj)->handle;
            }
        }
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_COLLECTION, FALSE);

    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        if (elem->obj == NULL)
        {
            OCI_CollInit(elem->con, (OCI_Coll **) &elem->obj,
                         (OCIColl  *) elem->handle,
                         elem->typinf->cols[0].typinf);
        }

        if (elem->obj != NULL)
        {
            res = OCI_CollAssign((OCI_Coll *) elem->obj, value);

            if (res == TRUE)
            {
                OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);

                elem->handle = ((OCI_Coll *) elem->obj)->handle;
            }
        }
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_OBJECT, FALSE);

    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        if (elem->obj == NULL)
        {
            OCI_ObjectInit(elem->con, (OCI_Object **) &elem->obj,
                           elem->handle, elem->typinf->cols[0].typinf,
                           NULL, -1, TRUE);
        }

        if (elem->obj != NULL)
        {
            res = OCI_ObjectAssign((OCI_Object *) elem->obj, value);

            if (res == TRUE)
            {
                OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);

                elem->handle = ((OCI_Object *) elem->obj)->handle;
            }
        }
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_LOB, FALSE);

    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        if (elem->obj == NULL)
        {
            OCI_LobInit(elem->con, (OCI_Lob **) &elem->obj,
                        (OCILobLocator *) elem->handle,
                        elem->typinf->cols[0].subtype);
        }

        if (elem->obj != NULL)
        {
            res = OCI_LobAssign((OCI_Lob *) elem->obj, value);

            if (res == TRUE)
            {
                OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);

                elem->handle = ((OCI_Lob *) elem->obj)->handle;
            }
        }
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_FILE, FALSE);

    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        if (elem->obj == NULL)
        {
            OCI_FileInit(elem->con, (OCI_File **) &elem->obj,
                         (OCILobLocator *) elem->handle,
                         elem->typinf->cols[0].subtype);
        }

        if (elem->obj != NULL)
        {
            res = OCI_FileAssign((OCI_File *) elem->obj, value);

            if (res == TRUE)
            {
                OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);

                elem->handle = ((OCI_Object *) elem->obj)->handle;
            }
        }
    }

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

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_REF, FALSE);

    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        if (elem->obj == NULL)
        {
            OCI_RefInit(elem->con,elem->typinf->cols[0].typinf,
                        (OCI_Ref **) &elem->obj, (OCIRef *) elem->handle);
        }

        if (elem->obj != NULL)
        {
            res = OCI_RefAssign((OCI_Ref *) elem->obj, value);

            if (res == TRUE)
            {
                OCI_ElemSetNullIndicator(elem, OCI_IND_NOTNULL);

                elem->handle = ((OCI_Ref *) elem->obj)->handle;
            }
        }
    }

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

    if (elem->pind != NULL)
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
