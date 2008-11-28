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
 * $Id: object.c, v 3.1.0 2008/10/26 07:50 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                            PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ObjectInit
 * ------------------------------------------------------------------------ */

OCI_Object * OCI_ObjectInit(OCI_Connection *con, OCI_Object **pobj,
                            void *handle, OCI_Schema *schema)
{
    OCI_Object * obj = NULL;
    boolean res      = TRUE;

    OCI_CHECK(pobj == NULL, NULL);

    if (*pobj == NULL)
        *pobj = (OCI_Object *) OCI_MemAlloc(OCI_IPC_OBJECT, sizeof(*obj),
                                          1, TRUE);

    if (*pobj != NULL)
    {
        obj = *pobj;

        obj->con    = con;
        obj->handle = handle;
        obj->nty    = schema;

        obj->objs = (void **) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, sizeof(void *),
                                           schema->nb_cols, TRUE);

        res = (obj->objs != NULL);

        if ((res == TRUE) && (obj->handle == NULL))
        {
            /* allocates handle for non fetched object */

            obj->hstate = OCI_OBJECT_ALLOCATED;

            OCI_CALL2
            (
                res, obj->con,

                OCI_ObjectNew(OCILib.env,  con->err, con->cxt,
                              (OCITypeCode) SQLT_NTY, obj->nty->tdo,
                              (dvoid *) NULL,
                              (OCIDuration) OCI_DURATION_SESSION,
                              (boolean) FALSE,
                              (dvoid **) &obj->handle)
           )
        }
        else
            obj->hstate = OCI_OBJECT_FETCHED_CLEAN;

        if (res == TRUE)
        {
            ub4 size = sizeof(obj->type);

            OCIObjectGetProperty(OCILib.env, con->err, obj->handle, 
                                 (OCIObjectPropId) OCI_OBJECTPROP_LIFETIME,
                                 (void *) &obj->type, &size);

        }
    }
    else
    {
        OCI_ObjectReset(obj);
    }

    /* check for failure */

    if (res == FALSE)
    {
        OCI_ObjectFree(obj);
        obj = NULL;
    }

    return obj;
}


/* ------------------------------------------------------------------------ *
 * OCI_ObjectReset
 * ------------------------------------------------------------------------ */

void OCI_ObjectReset(OCI_Object *obj)
{
    ub2 i;

    for (i = 0; i < obj->nty->nb_cols; i++)
    {
        if (obj->objs[i] != NULL)
        {
           ((OCI_Datatype *) obj->objs[i])->hstate = OCI_OBJECT_FETCHED_DIRTY;

            switch (obj->nty->cols[i].type)
            {
                case OCI_CDT_DATETIME:

                    OCI_DateFree((OCI_Date *) obj->objs[i]);
                    break;

                case OCI_CDT_LOB:

                    OCI_LobFree((OCI_Lob *) obj->objs[i]);
                    break;

                case OCI_CDT_FILE:

                    OCI_FileFree((OCI_File *) obj->objs[i]);
                    break;

                case OCI_CDT_OBJECT:

                    OCI_ObjectFree((OCI_Object *) obj->objs[i]);
                    break;

                case OCI_CDT_COLLECTION:

                    OCI_CollFree((OCI_Coll *) obj->objs[i]);;
                    break;

                case OCI_CDT_TIMESTAMP:

                    OCI_TimestampFree((OCI_Timestamp *) obj->objs[i]);
                    break;

                case OCI_CDT_INTERVAL:

                    OCI_IntervalFree((OCI_Interval *) obj->objs[i]);
                    break;
                case OCI_CDT_REF:

                    OCI_RefFree((OCI_Ref *) obj->objs[i]);
                    break;
            }
        }
    }
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetIndex
 * ------------------------------------------------------------------------ */

int OCI_ObjectGetIndex(OCI_Object *obj, const mtext *attr, int type)
{
    int res = -1;
    ub2 i;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, -1);
    OCI_CHECK_PTR(OCI_IPC_STRING, attr, -1);

    for (i = 0; i < obj->nty->nb_cols; i++)
    {
        OCI_Column *col = &obj->nty->cols[i];

        if (col->type == type  &&  mtscasecmp(col->name, attr) == 0)
        {
           res = (int) i;
           break;
        }
    }

    if (res == -1)
        OCI_ExceptionAttributeNotFound(obj->con, attr);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetAttr
 * ------------------------------------------------------------------------ */

boolean OCI_ObjectGetAttr(OCI_Object *obj, const mtext *attr, void ** p_value,
                          OCIInd* p_ind,  OCIType **p_tdo)
{
    boolean res = TRUE;

    void *ostr = NULL;
    int osize  = -1;

    ostr = OCI_GetInputMetaString(attr, &osize);

    OCI_CALL2
    (
        res, obj->con,

        OCIObjectGetAttr(OCILib.env, obj->con->err,  (dvoid *) obj->handle,
                        (dvoid *) NULL,  (OCIType *) obj->nty->tdo,
                        (CONST oratext **) &ostr,  (ub4 *) &osize,
                        (ub4) 1, (ub4 *) NULL, (ub4) 0, (OCIInd *) p_ind,
                        (dvoid **) NULL, (dvoid **) p_value, (OCIType **) p_tdo)
    )

    OCI_ReleaseMetaString(ostr);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetAttr
 * ------------------------------------------------------------------------ */

boolean OCI_ObjectSetAttr(OCI_Object *obj, const mtext *attr, void * value,
                          OCIInd ind)
{
    boolean res = TRUE;

    void *ostr = NULL;
    int osize  = -1;

    ostr = OCI_GetInputMetaString(attr, &osize);

    OCI_CALL2
    (
        res, obj->con,

        OCIObjectSetAttr(OCILib.env, obj->con->err, (dvoid *) obj->handle,
                        (dvoid *) NULL, (OCIType *) obj->nty->tdo,
                        (CONST oratext**) &ostr, (ub4 *) &osize,
                        (ub4) 1, (ub4 *) NULL, (ub4) 0, (OCIInd) ind,
                        (dvoid *) NULL, (dvoid *) value)
    )

    OCI_ReleaseMetaString(ostr);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetNumber
 * ------------------------------------------------------------------------ */

boolean OCI_ObjectSetNumber(OCI_Object *obj, const mtext *attr,
                            void *value, uword size, uword flag)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);

    if (OCI_ObjectGetIndex(obj, attr, OCI_CDT_NUMERIC) >= 0)
    {
        OCINumber num;

        res = OCI_NumberSet(obj->con, &num, value, size, flag) &&
              OCI_ObjectSetAttr(obj, attr, (void *) &num, 0);
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetNumber
 * ------------------------------------------------------------------------ */

boolean OCI_ObjectGetNumber(OCI_Object *obj, const mtext *attr, void *value,
                            uword size, uword flag)
{
    boolean res = FALSE;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);

    if (OCI_ObjectGetIndex(obj, attr, OCI_CDT_NUMERIC) >= 0)
    {
        OCIInd ind      = 0;
        OCIType *tdo    = NULL;
        OCINumber *num  = NULL;

        if (OCI_ObjectGetAttr(obj, attr, (void **) &num, &ind, &tdo) && num)
        {
            res = OCI_NumberGet(obj->con, num, value, size, flag);
        }
    }
    else if (OCI_ObjectGetIndex(obj, attr, OCI_CDT_TEXT) >= 0)
    {
        const mtext *fmt = OCI_GetDefaultFormatNumeric(obj->con);
        ub4 fmt_size     = (ub4) mtslen(fmt);
        dtext *data      = (dtext *) OCI_ObjectGetString(obj, attr);

        res = OCI_NumberGetFromStr(obj->con, value, size, flag, data,
                                   (int) dtslen(data),  fmt, fmt_size);
    }

    OCI_RESULT(res);

    return FALSE;
}

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ObjectCreate
 * ------------------------------------------------------------------------ */

OCI_Object * OCI_API OCI_ObjectCreate(OCI_Connection *con, OCI_Schema *schema)
{
    OCI_Object *obj = NULL;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);
    OCI_CHECK_PTR(OCI_IPC_SCHEMA, schema, NULL);

    obj = OCI_ObjectInit(con, &obj, NULL, schema);

    OCI_RESULT(obj != NULL);

    return obj;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectFree(OCI_Object *obj)
{
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);

    OCI_CHECK_OBJECT_FETCHED(obj, FALSE);

    /* if the object has sub-objects that have been fetched, we need to free
       these objects */

    if (obj->objs != NULL)
    {
        OCI_ObjectReset(obj);

        OCI_FREE(obj->objs);
    }

    if (obj->hstate == OCI_OBJECT_ALLOCATED)
    {
        OCI_OCIObjectFree(OCILib.env, obj->con->err,  obj->handle,
                          OCI_OBJECTFREE_NONULL);
    }

    OCI_FREE(obj->buf);
    OCI_FREE(obj);

    OCI_RESULT(TRUE);

    return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectAssign
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectAssign(OCI_Object *obj, OCI_Object *obj_src)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj_src, FALSE);

    OCI_CHECK_COMPAT(obj->con, obj->nty == obj_src->nty, FALSE);

    OCI_CALL2
    (
        res, obj->con,

        OCIObjectCopy(OCILib.env, obj->con->err, obj->con->cxt, obj_src->handle,
                      NULL, obj->handle, NULL, obj->nty->tdo,
                      OCI_DURATION_SESSION, OCI_DEFAULT)
    )

    if (res == TRUE)
    {
        obj->nty = obj_src->nty;

        OCI_ObjectReset(obj);
    }


    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetInt
 * ------------------------------------------------------------------------ */

short OCI_API OCI_ObjectGetShort(OCI_Object *obj, const mtext *attr)
{
    short value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_SHORT);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetUnsignedInt
 * ------------------------------------------------------------------------ */

unsigned short OCI_API OCI_ObjectGetUnsignedShort(OCI_Object *obj,
                                                  const mtext *attr)
{
    unsigned short value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_USHORT);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetInt
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ObjectGetInt(OCI_Object *obj, const mtext *attr)
{
    int value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_INT);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetUnsignedInt
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_ObjectGetUnsignedInt(OCI_Object *obj, const mtext *attr)
{
    unsigned int value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_UINT);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetBigInt
 * ------------------------------------------------------------------------ */

big_int OCI_API OCI_ObjectGetBigInt(OCI_Object *obj, const mtext *attr)
{
    big_int value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_BIGINT);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetUnsignedBigInt
 * ------------------------------------------------------------------------ */

big_uint OCI_API OCI_ObjectGetUnsignedBigInt(OCI_Object *obj, const mtext *attr)
{
    big_uint value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_BIGUINT);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetDouble
 * ------------------------------------------------------------------------ */

double OCI_API OCI_ObjectGetDouble(OCI_Object *obj, const mtext *attr)
{
    double value = 0.0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_DOUBLE);

    return value;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetString
 * ------------------------------------------------------------------------ */

const dtext * OCI_API OCI_ObjectGetString(OCI_Object *obj, const mtext *attr)
{
    const dtext *str  = NULL;
    boolean res       = FALSE;
    int index         = OCI_ObjectGetIndex(obj, attr, OCI_CDT_TEXT);

    if (index >= 0)
    {
        OCIInd ind        = 0;
        OCIType *tdo      = NULL;
        OCIString **value = NULL;

        res = OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo);

        if ((res == TRUE) && (value != NULL))
        {
            str = (dtext *) OCI_StringFromStringPtr(*value, &obj->buf, &obj->buflen);
        }
    }

    OCI_RESULT(res);

    return str;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetRaw
 * ------------------------------------------------------------------------ */

int OCI_API OCI_ObjectGetRaw(OCI_Object *obj, const mtext *attr, void *buffer,
                             unsigned int len)
{
    boolean res = FALSE;
    int index   = OCI_ObjectGetIndex(obj, attr, OCI_CDT_RAW);
    ub4 raw_len = 0;

    if (index >= 0)
    {
        OCIInd ind     = 0;
        OCIType *tdo   = NULL;
        OCIRaw **value = NULL;

        res = OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo);

        if ((res == TRUE) && (value != NULL))
        {
            OCI_CALL2
            (
                res, obj->con,

                OCIRawAllocSize(OCILib.env, obj->con->err, *value, (ub4*) &raw_len)
            )

            if (res == TRUE)
            {
                if (len > raw_len)
                    len = raw_len;

                memcpy(buffer, OCIRawPtr(OCILib.env, *value), len);
            }
        }
    }

    OCI_RESULT(res);

    return len;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetDate
 * ------------------------------------------------------------------------ */

OCI_Date * OCI_API OCI_ObjectGetDate(OCI_Object *obj, const mtext *attr)
{
    OCI_Date * date = NULL;
    boolean res     = FALSE;
    int index       = OCI_ObjectGetIndex(obj, attr, OCI_CDT_DATETIME);

    if (index >= 0)
    {
        OCIInd ind    = 0;
        OCIType *tdo  = NULL;

        date = OCI_DateInit(obj->con, (OCI_Date **) &obj->objs[index],
                            NULL, FALSE, FALSE);

        res = (date != NULL);

        if (res == TRUE)
            res = OCI_ObjectGetAttr(obj, attr, (void **) &date->handle, &ind, &tdo);
    }

    OCI_RESULT(res);

    return date;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetTimeStamp
 * ------------------------------------------------------------------------ */

OCI_Timestamp * OCI_API OCI_ObjectGetTimeStamp(OCI_Object *obj,
                                               const mtext *attr)
{
    OCI_Timestamp *tmsp = NULL;
    boolean res         = FALSE;
    int index           = OCI_ObjectGetIndex(obj, attr, OCI_CDT_TIMESTAMP);
    if (index >= 0)
    {
        OCIInd ind          = 0;
        OCIType *tdo        = NULL;
        OCIDateTime **value = NULL;

        res = OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo);

        if ((res == TRUE) && (value != NULL))
        {
           tmsp = OCI_TimestampInit(obj->con,
                                    (OCI_Timestamp **) &obj->objs[index],
                                    (OCIDateTime *) *value,
                                    obj->nty->cols[index].subtype);

           res = (tmsp != NULL);
        }
    }

    OCI_RESULT(res);

    return tmsp;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetInterval
 * ------------------------------------------------------------------------ */

OCI_Interval * OCI_API OCI_ObjectGetInterval(OCI_Object *obj, const mtext *attr)
{
    OCI_Interval *itv = NULL;
    boolean res       = FALSE;
    int index         = OCI_ObjectGetIndex(obj, attr, OCI_CDT_INTERVAL);

    if (index >= 0)
    {
        OCIInd ind          = 0;
        OCIType *tdo        = NULL;
        OCIInterval **value = NULL;

        res = OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo);

        if ((res == TRUE) && (value != NULL))
        {
            itv = OCI_IntervalInit(obj->con,
                                   (OCI_Interval **) &obj->objs[index],
                                   (OCIInterval *) *value,
                                   obj->nty->cols[index].subtype);

            res = (itv != NULL);
        }
    }

    OCI_RESULT(res);

    return itv;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetColl
 * ------------------------------------------------------------------------ */

OCI_Coll * OCI_API OCI_ObjectGetColl(OCI_Object *obj, const mtext *attr)
{
    OCI_Coll *coll = NULL;
    boolean res    = FALSE;
    int index      = OCI_ObjectGetIndex(obj, attr, OCI_CDT_COLLECTION);

    if (index >= 0)
    {
        OCIInd ind      = 0;
        OCIType *tdo    = NULL;
        OCIColl **value = NULL;

        res = OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo);

        if ((res == TRUE) && (value != NULL))
        {
            coll = OCI_CollInit(obj->con,
                                (OCI_Coll **) &obj->objs[index],
                                (OCIColl *) value,
                                obj->nty->cols[index].nty);

            res = (coll != NULL);
        }
    }

    OCI_RESULT(res);

    return coll;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetObject
 * ------------------------------------------------------------------------ */

OCI_Object * OCI_API OCI_ObjectGetObject(OCI_Object *obj, const mtext *attr)
{
    OCI_Object *obj2 = NULL;
    boolean res      = FALSE;
    int index        = OCI_ObjectGetIndex(obj, attr, OCI_CDT_OBJECT);

    if (index >= 0)
    {
        OCIInd ind    = 0;
        OCIType *tdo  = NULL;
        void *value   = NULL;

        res = OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo);

        if ((res == TRUE) && (value != NULL))
        {
            obj2 = OCI_ObjectInit(obj->con, (OCI_Object **) &obj->objs[index],
                                  value, obj->nty->cols[index].nty);

            res = (obj2 != NULL);
       }
    }

    OCI_RESULT(res);

    return obj2;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetLob
 * ------------------------------------------------------------------------ */

OCI_Lob * OCI_API OCI_ObjectGetLob(OCI_Object *obj, const mtext *attr)
{
    OCI_Lob *lob = NULL;
    boolean res  = FALSE;
    int index    = OCI_ObjectGetIndex(obj, attr, OCI_CDT_LOB);

    if (index >= 0)
    {
        OCIInd ind            = 0;
        OCIType *tdo          = NULL;
        OCILobLocator **value = NULL;

        res = OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo);

        if ((res == TRUE) && (value != NULL))
        {
            lob = OCI_LobInit(obj->con, (OCI_Lob **) &obj->objs[index],
                              *value, obj->nty->cols[index].subtype);

            res = (lob != NULL);
        }
    }

    OCI_RESULT(res);

    return lob;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetFile
 * ------------------------------------------------------------------------ */

OCI_File * OCI_API OCI_ObjectGetFile(OCI_Object *obj, const mtext *attr)
{
    OCI_File *file = NULL;
    boolean res    = FALSE;
    int index      = OCI_ObjectGetIndex(obj, attr, OCI_CDT_FILE);

    if (index >= 0)
    {
        OCIInd ind            = 0;
        OCIType *tdo          = NULL;
        OCILobLocator **value = NULL;

        res = OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo);

        if ((res == TRUE) && (value != NULL))
        {
            file = OCI_FileInit(obj->con, (OCI_File **) &obj->objs[index],
                                *value, obj->nty->cols[index].subtype);

            res = (file != NULL);
        }
    }

    OCI_RESULT(res);

    return file;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetRef
 * ------------------------------------------------------------------------ */

OCI_Ref * OCI_API OCI_ObjectGetRef(OCI_Object *obj, const mtext *attr)
{
    OCI_Ref *ref = NULL;
    boolean res  = FALSE;
    int index    = OCI_ObjectGetIndex(obj, attr, OCI_CDT_REF);

    if (index >= 0)
    {
        OCIInd ind     = 0;
        OCIType *tdo   = NULL;
        OCIRef **value = NULL;

        res = OCI_ObjectGetAttr(obj, attr, (void **) &value, &ind, &tdo);

        if ((res == TRUE) && (value != NULL))
        {
            ref = OCI_RefInit(obj->con, NULL, (OCI_Ref **) &obj->objs[index],
                              *value);

            res = (ref != NULL);
        }
    }

    OCI_RESULT(res);

    return ref;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetShort(OCI_Object *obj, const mtext *attr, short value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_SHORT);
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetUnsignedShort
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetUnsignedShort(OCI_Object *obj, const mtext *attr,
                                           unsigned short value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_USHORT);
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetInt(OCI_Object *obj, const mtext *attr, int value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_INT);
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetUnsignedInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetUnsignedInt(OCI_Object *obj, const mtext *attr,
                                        unsigned int value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_UINT);
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetBigInt(OCI_Object *obj, const mtext *attr,
                                    big_int value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_BIGINT);
}


/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetUnsignedBigInt
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetUnsignedBigInt(OCI_Object *obj, const mtext *attr,
                                            big_uint value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_BIGUINT);
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetDouble
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetDouble(OCI_Object *obj, const mtext *attr,
                                    double value)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_DOUBLE);
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetString
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetString(OCI_Object *obj, const mtext *attr,
                                    const dtext *value)
{
    boolean res = TRUE;

    if (value == NULL)
    {
        res = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        if (OCI_ObjectGetIndex(obj, attr, OCI_CDT_TEXT) >= 0)
        {
            OCIString *str = NULL;

            res = OCI_StringToStringPtr(&str, obj->con->err, (void *) value,
                                        &obj->buf, &obj->buflen);

            if (res == TRUE)
                res = OCI_ObjectSetAttr(obj, attr, (void *) str, 0);
        }
        else
            res = FALSE;
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetRaw
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetRaw(OCI_Object *obj, const mtext *attr,
                                 void* value, unsigned int len)
{
    boolean res = TRUE;

    if (value == NULL)
    {
        res = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        OCI_CHECK_MIN(obj->con, NULL, len, 1, FALSE);

        if (OCI_ObjectGetIndex(obj, attr, OCI_CDT_RAW) >= 0)
        {
            OCIRaw *raw = NULL;

            OCI_CALL2
            (
                res, obj->con,

                OCIRawAssignBytes(OCILib.env, obj->con->err, (ub1*) value, len,
                                  &raw)
            )

            if (res == TRUE)
                res = OCI_ObjectSetAttr(obj, attr, (void *) raw, 0);
        }
        else
            res = FALSE;
    }

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectSetNull
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectSetNull(OCI_Object *obj, const mtext *attr)
{
    boolean res = TRUE;
    void *ostr  = NULL;
    int osize   = -1;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, attr, FALSE);

    ostr = OCI_GetInputMetaString(attr, &osize);

    OCI_CALL2
    (
        res, obj->con,

        OCIObjectSetAttr(OCILib.env, obj->con->err, obj->handle, NULL,
                         obj->nty->tdo, (CONST text**) &ostr, (ub4 *) &osize,
                         (ub4) 1, (ub4 *) NULL, (ub4) 0,  OCI_IND_NULL,
                         (void *)  NULL, (void *) NULL)
    )

    OCI_ReleaseMetaString(ostr);

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetSchema
 * ------------------------------------------------------------------------ */

OCI_Schema * OCI_API OCI_ObjectGetSchema(OCI_Object *obj)
{
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, NULL);

    OCI_RESULT(TRUE);

    return obj->nty;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetType
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_ObjectGetType(OCI_Object *obj)
{
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return (unsigned int) obj->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_ObjectGetSelfRef
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_ObjectGetSelfRef(OCI_Object *obj, OCI_Ref *ref)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);
    OCI_CHECK_PTR(OCI_IPC_REF   , ref, FALSE);

    OCI_CHECK_COMPAT(obj->con, obj->nty == ref->nty, FALSE);

    OCI_CALL2
    (
        res, obj->con, 

        OCIObjectGetObjectRef(OCILib.env, obj->con->err, obj->handle, ref->handle)
    )

    if (res == TRUE)
    {
        OCI_ObjectFree(ref->obj);       
        ref->obj = NULL;
    }

    OCI_RESULT(res);

    return res;
}
