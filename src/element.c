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
   |               Copyright (c) 2007-2008 Vincent ROGIER                 |
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
 * $Id: element.c, v 3.1.0 2008/10/26 07:50 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ElemInit
 * ------------------------------------------------------------------------ */

OCI_Elem * OCI_ElemInit(OCI_Connection *con, OCI_Elem **pelem, void *handle, 
                        OCIInd *pind, OCI_Column *col)
{
    OCI_Elem *elem = NULL;
    boolean res    = TRUE;

    OCI_CHECK(pelem == NULL, NULL);

    if (*pelem == NULL)
        *pelem = (OCI_Elem *) OCI_MemAlloc(OCI_IPC_ELEMENT, sizeof(*elem), 1, TRUE);

    if (*pelem != NULL)
    {
        elem = *pelem;

        elem->con    = con;
        elem->handle = handle;
        elem->ind    = pind;
        elem->col    = col;
    
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
  
    if (elem->col->type == OCI_CDT_NUMERIC)
    {
        OCINumber *num  = (OCINumber *) elem->handle;
     
        res = OCI_NumberSet(elem->con, num, value, size, flag);
    }

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
    
    if (elem->col->type == OCI_CDT_NUMERIC)
    {
        OCINumber *num  = (OCINumber *) elem->handle;

        res = OCI_NumberGet(elem->con, num, value, size, flag);
    }
    else if (elem->col->type == OCI_CDT_TEXT)
    {
        const mtext *fmt = OCI_GetDefaultFormatNumeric(elem->con);
        ub4 fmt_size     = (ub4) mtslen(fmt);
        dtext *data      = (dtext *) OCI_ElemGetString(elem);

        res = OCI_NumberGetFromStr(elem->con, value, size, flag, data, 
                                   (int) dtslen(data),  fmt, fmt_size);
    }

    OCI_RESULT(res);

    return FALSE;
}

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ElemCreate
 * ------------------------------------------------------------------------ */

OCI_Elem * OCI_API OCI_ElemCreate(OCI_Connection *con)
{
    OCI_Elem *elem = NULL;

    OCI_CHECK_INITIALIZED(NULL);

    elem = OCI_ElemInit(con, &elem, (OCIColl *) NULL, (OCIInd *) NULL, NULL);
 
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
       ((OCI_Datatype *) (elem->obj))->hstate = OCI_OBJECT_FETCHED_DIRTY;

        switch (elem->col->type)
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
    OCI_Date *date = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);

    if (elem->col->type == OCI_CDT_DATETIME)
    {
        date = OCI_DateInit(elem->con, (OCI_Date **) &elem->obj, 
                            (OCIDate* ) elem->handle , FALSE, FALSE);
    }

    OCI_RESULT(date != NULL);

    return date;
}
/* ------------------------------------------------------------------------ *
 * OCI_ElemGetTimeStamp
 * ------------------------------------------------------------------------ */

OCI_Timestamp * OCI_API  OCI_ElemGetTimeStamp(OCI_Elem *elem)
{
    OCI_Timestamp *tmsp = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);

    if (elem->col->type == OCI_CDT_TIMESTAMP)
    {
        tmsp = OCI_TimestampInit(elem->con, (OCI_Timestamp **) &elem->obj, 
                            (OCIDateTime *) elem->handle, elem->col->subtype);
    }

    OCI_RESULT(tmsp != NULL);

    return tmsp;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetInterval
 * ------------------------------------------------------------------------ */

OCI_Interval * OCI_API OCI_ElemGetInterval(OCI_Elem *elem)
{
    OCI_Interval *itv = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);

    if (elem->col->type == OCI_CDT_INTERVAL)
    {
        itv = OCI_IntervalInit(elem->con, (OCI_Interval **) &elem->obj, 
                               (OCIInterval *) elem->handle, elem->col->subtype);
    }

    OCI_RESULT(itv != NULL);

    return itv;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetLob
 * ------------------------------------------------------------------------ */

OCI_Lob * OCI_API  OCI_ElemGetLob(OCI_Elem *elem)
{
    OCI_Lob *lob = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);

    if (elem->col->type == OCI_CDT_LOB)
    {
        OCILobLocator *loc = *(OCILobLocator **) elem->handle;
        
        lob = OCI_LobInit(elem->con, (OCI_Lob **) &elem->obj, loc,
                          elem->col->subtype);
    }

    OCI_RESULT(lob != NULL);

    return lob;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetFile
 * ------------------------------------------------------------------------ */

OCI_File * OCI_API  OCI_ElemGetFile(OCI_Elem *elem)
{
    OCI_File *file = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);

    if (elem->col->type == OCI_CDT_FILE)
    {
        OCILobLocator *loc = *(OCILobLocator **) elem->handle;

        file = OCI_FileInit(elem->con, (OCI_File **) &elem->obj, loc, 
                            elem->col->subtype);
    }


    OCI_RESULT(file != NULL);

    return file;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetRef
 * ------------------------------------------------------------------------ */

OCI_Ref * OCI_API  OCI_ElemGetRef(OCI_Elem *elem)
{
    OCI_Ref *ref = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);

    if (elem->col->type == OCI_CDT_REF)
    {
        OCIRef *handle = *(OCIRef **) elem->handle;
        
        ref = OCI_RefInit(elem->con, NULL, (OCI_Ref **) &elem->obj, handle);
    }

    OCI_RESULT(ref != NULL);

    return ref;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetObject
 * ------------------------------------------------------------------------ */

OCI_Object * OCI_API OCI_ElemGetObject(OCI_Elem *elem)
{
    OCI_Object *obj = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);

    if (elem->col->type == OCI_CDT_OBJECT)
    {
        obj = OCI_ObjectInit(elem->con, (OCI_Object **) &elem->obj,
                             (void *) elem->handle, elem->col->nty);

    }

    OCI_RESULT(obj != NULL);

    return obj;
}

/* ------------------------------------------------------------------------ *
 * OCI_ElemGetColl
 * ------------------------------------------------------------------------ */

OCI_Coll * OCI_API OCI_ElemGetColl(OCI_Elem *elem)
{
    OCI_Coll *coll = NULL;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, NULL);

    if (elem->col->type == OCI_CDT_COLLECTION)
    {
        coll = OCI_CollInit(elem->con, (OCI_Coll **) &elem->obj, 
                            elem->handle, NULL);
    }

    OCI_RESULT(coll != NULL);

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
 * OCI_ElemSetUnsignedBigIn
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

    if (value == NULL)
    {
        res = OCI_ElemSetNull(elem);
    }
    else if (elem->col->type == OCI_CDT_TEXT)
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
 * OCI_ElemIsNull
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ElemIsNull(OCI_Elem *elem)
{
    boolean res  = FALSE;
    boolean null = TRUE;

    OCI_CHECK_PTR(OCI_IPC_ELEMENT, elem, FALSE);

    if (elem->ind != NULL)
    {     
        if (*elem->ind == OCI_IND_NULL)
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
        *elem->ind = -1;
        res = TRUE;
    }

    OCI_RESULT(res);
   
    return res;
}

