/*
   +----------------------------------------------------------------------+   
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2009 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+ 
*/

/* ------------------------------------------------------------------------ *
 * $Id: element.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ElemInit
 * ------------------------------------------------------------------------ */

OCI_Elem * OCI_ElemInit(OCI_Connection *con, OCI_Elem **pelem, void *handle, 
                        OCIInd *pind, OCI_TypeInfo *typinf)
{
    OCI_Elem *elem = NULL;
    boolean res    = TRUE;

    OCI_CHECK(pelem == NULL, NULL);

    if (*pelem == NULL)
        *pelem = (OCI_Elem *) OCI_MemAlloc(OCI_IPC_ELEMENT, sizeof(*elem), 1, TRUE);

    if (*pelem != NULL)
    {
        elem = *pelem;

        elem->con     = con;
        elem->handle  = handle;
        elem->ind     = pind;
        elem->typinf  = typinf;
        elem->init    = FALSE;
    
        if (elem->handle == NULL)
            elem->hstate = OCI_OBJECT_ALLOCATED;
        else
            elem->hstate = OCI_OBJECT_FETCHED_CLEAN;
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

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetNumber
 * ------------------------------------------------------------------------ */

boolean OCI_ElemSetNumber(OCI_Elem  *elem, void *value, uword size, uword flag)
{
    boolean res = FALSE;
        
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_NUMERIC, FALSE);
  
    res = OCI_NumberSet(elem->con, (OCINumber *) elem->handle, value, size, flag);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetNumber
 * ------------------------------------------------------------------------ */

boolean OCI_ElemGetNumber(OCI_Elem *elem, void *value, uword size, uword flag)
{
    boolean res = FALSE;
    
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    
    if (elem->typinf->cols[0].type == OCI_CDT_NUMERIC)
    {
        OCINumber *num  = (OCINumber *) elem->handle;

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

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ElemCreate
 * ------------------------------------------------------------------------ */

OCI_Elem * OCI_API OCI_ElemCreate(OCI_TypeInfo *typinf)
{
    OCI_Elem *elem = NULL;

    OCI_CHECK_INITIALIZED(NULL);
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);

    elem = OCI_ElemInit(typinf->con, &elem, NULL, (OCIInd *) NULL, typinf);
                        
    OCI_RESULT(elem != NULL);

    return elem;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemFree(OCI_Elem *elem)
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
        }
    }

    OCI_FREE(elem->buf);
    OCI_FREE(elem);

    OCI_RESULT(TRUE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetShort
 * ------------------------------------------------------------------------ */

short OCI_API OCI_ElemGetShort(OCI_Elem *elem)
{
    short value = 0;
    
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, 0);

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(short), 
                      (uword) OCI_NUM_SHORT);
  
    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetUnsignedShort
 * ------------------------------------------------------------------------ */

unsigned short OCI_API OCI_ElemGetUnsignedShort(OCI_Elem *elem)
{
    unsigned short value = 0;
    
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, 0);

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(unsigned short),
                      (uword) OCI_NUM_USHORT);
 
    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetInt
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ElemGetInt(OCI_Elem *elem)
{
    int value = 0;
    
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, 0);

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(value),
                      (uword) OCI_NUM_INT);
 
    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetUnsignedInt
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_ElemGetUnsignedInt(OCI_Elem *elem)
{
    unsigned int value = 0;
    
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, 0);

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(value),
                      (uword) OCI_NUM_UINT);
 
    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetBigInt
 * ------------------------------------------------------------------------ */

big_int OCI_API OCI_ElemGetBigInt(OCI_Elem *elem)
{
    big_int value = 0;
    
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, 0);

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(big_int),
                      (uword) OCI_NUM_BIGINT);
 
    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetUnsignedBigInt
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_ElemGetUnsignedBigInt(OCI_Elem *elem)
{
    big_uint value = 0;
    
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, 0);

    OCI_ElemGetNumber(elem, (void *) &value,  (uword) sizeof(big_uint),
                      (uword) OCI_NUM_BIGUINT);
 
    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetDouble
 * ------------------------------------------------------------------------ */

double OCI_API OCI_ElemGetDouble(OCI_Elem *elem)
{
    double value = 0.0;
    
    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, 0.0);

    OCI_ElemGetNumber(elem, (void *) &value, (uword) sizeof(double),
                      (uword) OCI_NUM_DOUBLE);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetString
 * ------------------------------------------------------------------------ */

const dtext * OCI_API OCI_ElemGetString(OCI_Elem *elem)
{
    const dtext *str  = NULL;
    boolean res       = FALSE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_TEXT, NULL);

    if (elem->handle != NULL)
    {
        res = TRUE;

        str = (dtext *) OCI_StringFromStringPtr(*(OCIString **) elem->handle,
                                                &elem->buf, &elem->buflen);
    }

    OCI_RESULT(res);

    return str;    
}


/* ------------------------------------------------------------------------ *
 * OCI_ElemGetRaw
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_ElemGetRaw(OCI_Elem *elem, void *value, unsigned int len)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, 0);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_RAW, 0);

    if (elem->handle != NULL)
    {
        OCIRaw *raw = *(OCIRaw **) elem->handle;
        ub4 raw_len = 0;
    
        OCI_CALL2
        (
            res, elem->con, 
            
            OCIRawAllocSize(OCILib.env, elem->con->err, raw, (ub4*) &raw_len)
        )

        if (res == TRUE)
        {
            if (len > raw_len)
                len = raw_len;

            memcpy(value, OCIRawPtr(OCILib.env, raw), len);
        }
    }

    OCI_RESULT(res);

    return len; 
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetDate
 * ------------------------------------------------------------------------ */

OCI_Date * OCI_API  OCI_ElemGetDate(OCI_Elem *elem)
{
    boolean res    = TRUE;
    OCI_Date *date = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_DATETIME, NULL);

    if (*(elem->ind) == OCI_IND_NOTNULL)
    {
        OCIDate *handle = (OCIDate *) elem->handle;

        if (elem->init == FALSE)
        {
            date = OCI_DateInit(elem->con, (OCI_Date **) &elem->obj, handle , 
                                FALSE, FALSE);

            elem->init = (date != NULL);
        }
        else
            date = (OCI_Date *) elem->obj;

        res = elem->init;
    }

    OCI_RESULT(res);

    return date;
}
/* ------------------------------------------------------------------------ *
 * OCI_ElemGetTimeStamp
 * ------------------------------------------------------------------------ */

OCI_Timestamp * OCI_API  OCI_ElemGetTimeStamp(OCI_Elem *elem)
{
    boolean res = TRUE;
    OCI_Timestamp *tmsp = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_TIMESTAMP, NULL);

    if (*(elem->ind) == OCI_IND_NOTNULL)
    {
        OCIDateTime *handle = (OCIDateTime *) elem->handle;

        if (elem->init == FALSE)
        {
            tmsp = OCI_TimestampInit(elem->con, (OCI_Timestamp **) &elem->obj, 
                                     handle, elem->typinf->cols[0].subtype);

            elem->init = (tmsp != NULL);
        }
        else
            tmsp = (OCI_Timestamp *) elem->obj;

        res = elem->init;
    }

    OCI_RESULT(res);

    return tmsp;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetInterval
 * ------------------------------------------------------------------------ */

OCI_Interval * OCI_API OCI_ElemGetInterval(OCI_Elem *elem)
{
    boolean res = TRUE;
    OCI_Interval *itv = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_INTERVAL, NULL);

    if (*(elem->ind) == OCI_IND_NOTNULL)
    {
        OCIInterval *handle = (OCIInterval *) elem->handle;

        if (elem->init == FALSE)
        {
            itv = OCI_IntervalInit(elem->con, (OCI_Interval **) &elem->obj, 
                                   handle, elem->typinf->cols[0].subtype);

            elem->init = (itv != NULL);
        }
        else
            itv = (OCI_Interval *) elem->obj;

        res = elem->init;
    }

    OCI_RESULT(res);

    return itv;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetLob
 * ------------------------------------------------------------------------ */

OCI_Lob * OCI_API  OCI_ElemGetLob(OCI_Elem *elem)
{
    boolean res  = TRUE;
    OCI_Lob *lob = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_LOB, NULL);

    if (*(elem->ind) == OCI_IND_NOTNULL)
    {
        OCILobLocator *handle = *(OCILobLocator **) elem->handle;
        
        if (elem->init == FALSE)
        {
            lob = OCI_LobInit(elem->con, (OCI_Lob **) &elem->obj, handle,
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

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetFile
 * ------------------------------------------------------------------------ */

OCI_File * OCI_API  OCI_ElemGetFile(OCI_Elem *elem)
{
    boolean res    = TRUE;
    OCI_File *file = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_FILE, NULL);

    if (*(elem->ind) == OCI_IND_NOTNULL)
    {
        OCILobLocator *handle = *(OCILobLocator **) elem->handle;

        if (elem->init == FALSE)
        {
            file = OCI_FileInit(elem->con, (OCI_File **) &elem->obj, handle, 
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

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetRef
 * ------------------------------------------------------------------------ */

OCI_Ref * OCI_API  OCI_ElemGetRef(OCI_Elem *elem)
{
    boolean res  = TRUE;
    OCI_Ref *ref = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_REF, NULL);

    if (*(elem->ind) == OCI_IND_NOTNULL)
    {
        OCIRef *handle = *(OCIRef **) elem->handle;
        
        if (elem->init == FALSE)
        {
            ref = OCI_RefInit(elem->con, elem->typinf->cols[0].typinf, 
                              (OCI_Ref **) &elem->obj, handle);

            elem->init = (ref != NULL);
        }
        else
            ref = (OCI_Ref *) elem->obj;

        res = elem->init;
    }

    OCI_RESULT(res);

    return ref;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetObject
 * ------------------------------------------------------------------------ */

OCI_Object * OCI_API OCI_ElemGetObject(OCI_Elem *elem)
{
    boolean res = TRUE;
    OCI_Object *obj = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_OBJECT, NULL);

    if (*(elem->ind) == OCI_IND_NOTNULL)
    {
        void * handle = elem->handle;

        if (elem->init == FALSE)
        {
            obj = OCI_ObjectInit(elem->con, (OCI_Object **) &elem->obj,
                                 handle, elem->typinf->cols[0].typinf, NULL, -1, TRUE);

            elem->init = (obj != NULL);
        }
        else
            obj = (OCI_Object *) elem->obj;

        res = elem->init;
    }

    OCI_RESULT(res);

    return obj;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetColl
 * ------------------------------------------------------------------------ */

OCI_Coll * OCI_API OCI_ElemGetColl(OCI_Elem *elem)
{
    boolean res    = TRUE;
    OCI_Coll *coll = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_COLLECTION, NULL);

    if (*(elem->ind) == OCI_IND_NOTNULL)
    {
        OCIColl *handle = (OCIColl *) elem->handle;

        if (elem->init == FALSE)
        {
            coll = OCI_CollInit(elem->con, (OCI_Coll **) &elem->obj, handle, 
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

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetShort(OCI_Elem *elem, short value)
{
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), 
                             (uword) OCI_NUM_SHORT);
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetUnsignedShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetUnsignedShort(OCI_Elem *elem, unsigned short value)
{
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), 
                             (uword) OCI_NUM_USHORT);
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetInt(OCI_Elem *elem, int value)
{
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), 
                             (uword) OCI_NUM_INT);
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetUnsignedInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetUnsignedInt(OCI_Elem *elem, unsigned int value)
{
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), 
                             (uword) OCI_NUM_UINT);
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetBigInt(OCI_Elem *elem, big_int value)
{
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), 
                             (uword) OCI_NUM_BIGINT);
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetUnsignedBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetUnsignedBigInt(OCI_Elem *elem, big_uint value)
{
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), 
                             (uword) OCI_NUM_BIGUINT);
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetDouble
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetDouble(OCI_Elem *elem, double value)
{
    return OCI_ElemSetNumber(elem, (void *) &value, (uword) sizeof(value), 
                             (uword) OCI_NUM_DOUBLE);
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetString
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetString(OCI_Elem *elem, const dtext *value)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_TEXT, FALSE);

    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        OCIString **str = (OCIString **) elem->handle;

        res = OCI_StringToStringPtr(str, elem->con->err, (void *) value, 
                                    &elem->buf, &elem->buflen);
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetRaw
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetRaw(OCI_Elem *elem, void* value, unsigned int len)
{
    boolean res  = TRUE;

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
                              len, (OCIRaw **) elem->handle)
        )
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetDate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetDate(OCI_Elem *elem, OCI_Date *value)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_DATETIME, FALSE);
  
    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        OCI_Date *date  = (OCI_Date *) elem->obj;
        OCIDate *handle = (OCIDate *) elem->handle; 

        if (date == NULL)
        {
            date = OCI_DateInit(elem->con, (OCI_Date **) &elem->obj, 
                                handle, TRUE, FALSE);
        }

        if (date != NULL)
        {
            res = OCI_DateAssign(date, value);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetTimestamp
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetTimestamp(OCI_Elem *elem, OCI_Timestamp *value)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_TIMESTAMP, FALSE);
  
    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        OCI_Timestamp *tmsp = (OCI_Timestamp *) elem->obj;
        OCIDateTime *handle = (OCIDateTime   *) elem->handle; 

        if (tmsp == NULL)
        {
            tmsp = OCI_TimestampInit(elem->con, (OCI_Timestamp **) &elem->obj,
                                     handle, elem->typinf->cols[0].subtype);
        }

        if (tmsp != NULL)
        {
            res = OCI_TimestampAssign(tmsp, value);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetInterval
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetInterval(OCI_Elem *elem, OCI_Interval *value)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_INTERVAL, FALSE);
 
    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        OCI_Interval *itv   = (OCI_Interval *) elem->obj;
        OCIInterval *handle = (OCIInterval  *) elem->handle;

        if (itv == NULL)
        {
            itv = OCI_IntervalInit(elem->con, (OCI_Interval **) &elem->obj,
                                   handle, elem->typinf->cols[0].subtype);
        }

        if (itv != NULL)
        {
            res = OCI_IntervalAssign(itv, value);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetColl
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetColl(OCI_Elem *elem, OCI_Coll *value)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_COLLECTION, FALSE);
  
    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        OCI_Coll *coll  = (OCI_Coll *) elem->obj;
        OCIColl *handle = (OCIColl  *) elem->handle; 

        if (coll == NULL)
        {
            coll = OCI_CollInit(elem->con, (OCI_Coll **) &elem->obj,
                                handle, elem->typinf->cols[0].typinf);
        }

        if (coll != NULL)
        {
            res = OCI_CollAssign(coll, value);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetObject
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetObject(OCI_Elem *elem, OCI_Object *value)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_OBJECT, FALSE);
  
    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        OCI_Object *obj = (OCI_Object *) elem->obj;
        void *handle    = elem->handle;

        if (obj == NULL)
        {
            obj = OCI_ObjectInit(elem->con, (OCI_Object **) &elem->obj,
                                 handle, elem->typinf->cols[0].typinf, NULL, -1, TRUE);
        }

        if (obj != NULL)
        {
            elem->handle = obj->handle;

            res = OCI_ObjectAssign(obj, value);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetLob
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetLob(OCI_Elem *elem, OCI_Lob *value)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_LOB, FALSE);
  
    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        OCI_Lob *lob = (OCI_Lob *) elem->obj;
        OCILobLocator *handle = *(OCILobLocator **) elem->handle;

        if (lob == NULL)
        {
            lob = OCI_LobInit(elem->con, (OCI_Lob **) &elem->obj,
                              handle, elem->typinf->cols[0].subtype);
        }

        if (lob != NULL)
        {
            res = OCI_LobAssign(lob, value);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetFile
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetFile(OCI_Elem *elem, OCI_File *value)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_FILE, FALSE);
  
    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        OCI_File *file =  (OCI_File *) elem->obj;
        OCILobLocator *handle = *(OCILobLocator **) elem->handle;

        if (file == NULL)
        {
            file = OCI_FileInit(elem->con, (OCI_File **) &elem->obj,
                                handle, elem->typinf->cols[0].subtype);
        }

        if (file != NULL)
        {
            res = OCI_FileAssign(file, value);
        }
    }

    OCI_RESULT(res);

    return res;
}


/* ------------------------------------------------------------------------ *
 * OCI_ElemSetRef
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetRef(OCI_Elem *elem, OCI_Ref *value)
{
    boolean res  = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);
    OCI_CHECK_COMPAT(elem->con, elem->typinf->cols[0].type == OCI_CDT_REF, FALSE);
  
    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else
    {
        OCI_Ref *ref    =  (OCI_Ref *) elem->obj;
        OCIRef * handle = *(OCIRef **) elem->handle;

        if (ref == NULL)
        {
            ref = OCI_RefInit(elem->con,elem->typinf->cols[0].typinf, 
                              (OCI_Ref **) &elem->obj, handle);
        }

        if (ref != NULL)
        {
            res = OCI_RefAssign(ref, value);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemIsNull
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemIsNull(OCI_Elem *elem)
{
    boolean res  = FALSE;
    boolean null = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);

    if (elem->ind != NULL)
    {     
        if (*(elem->ind) == OCI_IND_NULL)
            null = TRUE;

        res = TRUE;
    }

    OCI_RESULT(res);

    return null;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemSetNull
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemSetNull(OCI_Elem *elem)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);

    if (elem->ind != NULL)
    {
        *(elem->ind) = OCI_IND_NULL;
        res = TRUE;
    }

    OCI_RESULT(res);
   
    return res;
}
