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
 * $Id: object.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                            PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetIndicatorOffset
 * --------------------------------------------------------------------------------------------- */

ub2 OCI_ObjectGetIndOffset
(
    OCI_TypeInfo *typinf,
    int           index
)
{
    ub2 i = 0, j = 1;

    for (i = 0; i < index; i++)
    {
        if (typinf->cols[i].type == OCI_CDT_OBJECT)
        {
            j += OCI_ObjectGetIndOffset(typinf->cols[i].typinf,
                                        typinf->cols[i].typinf->nb_cols);
        }
        else
        {
            j++;
        }

    }

    return j;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetStructSize
 * --------------------------------------------------------------------------------------------- */

size_t OCI_ObjectGetStructSize
(
    OCI_TypeInfo *typinf
)
{
    size_t size1 = 0;
    size_t size2 = 0;

    int type1 = 0;
    int type2 = 0;

    ub2 i;

    boolean align = FALSE;

    size_t size = 0;

    if (typinf->struct_size != 0)
    {
        size = typinf->struct_size;
    }
    else
    {
        for (i = 0; i < typinf->nb_cols; i++)
        {
            align = FALSE;

            if (i > 0)
            {
                size1 = size2;
                type1 = type2;

                typinf->offsets[i] = (int) size;
            }
            else
            {
                OCI_ObjectGetAttrInfo(typinf, i, &size1, &type1);

                typinf->offsets[i] = 0;
            }

            OCI_ObjectGetAttrInfo(typinf, i+1, &size2, &type2);

            switch (OCI_OFFSET_PAIR(type1, type2))
            {
                case OCI_OFFSET_PAIR(OCI_OFT_NUMBER, OCI_OFT_POINTER):
                case OCI_OFFSET_PAIR(OCI_OFT_DATE,   OCI_OFT_POINTER):
                case OCI_OFFSET_PAIR(OCI_OFT_OBJECT, OCI_OFT_POINTER):

                    align = TRUE;
                    break;
            }

            size += size1;

            if (align)
            {
                size = ROUNDUP(size, OCI_DEF_ALIGN);
            }
        }

        typinf->struct_size = size + size2;
    }

    return size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetAttrInfo
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ObjectGetAttrInfo
(
    OCI_TypeInfo *typinf,
    int           index,
    size_t       *p_size,
    int          *p_type
)
{
    if (index >= typinf->nb_cols)
    {
        *p_size = 0;
        *p_type = 0;

        return FALSE;
    }

    switch (typinf->cols[index].type)
    {
        case OCI_CDT_NUMERIC:

            *p_size = sizeof(OCINumber);
            *p_type = OCI_OFT_NUMBER;
            break;

        case OCI_CDT_DATETIME:

            *p_size = sizeof(OCIDate);
            *p_type = OCI_OFT_DATE;
            break;

        case OCI_CDT_OBJECT:

            *p_size = OCI_ObjectGetStructSize(typinf->cols[index].typinf);
            *p_type = OCI_OFT_OBJECT;
            break;

        default:

            *p_size = sizeof(void *);
            *p_type = OCI_OFT_POINTER;
    }

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectInit
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OCI_ObjectInit
(
    OCI_Connection *con,
    OCI_Object    **pobj,
    void           *handle,
    OCI_TypeInfo   *typinf,
    OCI_Object     *parent,
    int             index,
    boolean         reset
)
{
    OCI_Object * obj = NULL;
    boolean res      = TRUE;

    OCI_CHECK(pobj == NULL, NULL);

    if (*pobj == NULL)
        *pobj = (OCI_Object *) OCI_MemAlloc(OCI_IPC_OBJECT, sizeof(*obj),
                                            (size_t) 1, TRUE);

    if (*pobj != NULL)
    {
        obj = *pobj;

        obj->con    = con;
        obj->handle = handle;
        obj->typinf = typinf;

        if (obj->objs == NULL)
        {
            obj->objs = (void **) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY,
                                               sizeof(void *),
                                               (size_t) typinf->nb_cols,
                                               TRUE);
        }
        else
        {
            OCI_ObjectReset(obj);
        }

        res = (obj->objs != NULL);

        if (((res == TRUE)) &&
            ((obj->handle == NULL) || (obj->hstate == OCI_OBJECT_ALLOCATED_ARRAY)))
        {
            /* allocates handle for non fetched object */

            if (obj->hstate != OCI_OBJECT_ALLOCATED_ARRAY)
            {
                obj->hstate = OCI_OBJECT_ALLOCATED;
            }

            OCI_CALL2
            (
                res, obj->con,

                OCI_ObjectNew(OCILib.env,  con->err, con->cxt,
                              (OCITypeCode) SQLT_NTY, obj->typinf->tdo,
                              (dvoid *) NULL,
                              (OCIDuration) OCI_DURATION_SESSION,
                              (boolean) TRUE,
                              (dvoid **) &obj->handle)
            )
        }
        else
        {
            obj->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }

        if ((res == TRUE) && (obj->type == 0))
        {
            ub4 size = sizeof(obj->type);

            /* calling OCIObjectGetProperty() on objects that are attributes of
               parent objects leads to a segfault on MS Windows !
               We need to report that to Oracle! Because sub objects always are
               values, if the parent indicator array is not null, let's assign
               the object type properties ourselves */

            if (parent == NULL)
            {
                OCIObjectGetProperty(OCILib.env, con->err, obj->handle,
                                     (OCIObjectPropId) OCI_OBJECTPROP_LIFETIME,
                                     (void *) &obj->type, &size);
            }
            else
            {
                obj->type = OCI_OBJECT_VALUE;
            }
        }

        if ((res == TRUE) && ((reset == TRUE) || (obj->tab_ind == NULL)))
        {
            if (parent == NULL)
            {
                OCI_CALL2
                (
                    res, obj->con,

                    OCIObjectGetInd(OCILib.env, obj->con->err,
                                    (dvoid *) obj->handle,
                                    (dvoid **) &obj->tab_ind)
                )
            }
            else
            {
                obj->tab_ind = parent->tab_ind;
                obj->idx_ind = parent->idx_ind + OCI_ObjectGetIndOffset(parent->typinf, index);
            }
        }
    }
    else
        res = FALSE;

    /* check for failure */

    if (res == FALSE)
    {
        OCI_ObjectFree(obj);
        obj = NULL;
    }

    return obj;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectReset
 * --------------------------------------------------------------------------------------------- */

void OCI_ObjectReset
(
    OCI_Object *obj
)
{
    ub2 i;

    for (i = 0; i < obj->typinf->nb_cols; i++)
    {
        if (obj->objs[i] != NULL)
        {
            OCI_Datatype * data = (OCI_Datatype *) obj->objs[i];

            if (data->hstate == OCI_OBJECT_FETCHED_CLEAN)
                data->hstate =  OCI_OBJECT_FETCHED_DIRTY;

            switch (obj->typinf->cols[i].type)
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

            obj->objs[i] = NULL;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetAttrIndex
 * --------------------------------------------------------------------------------------------- */

int OCI_ObjectGetAttrIndex
(
    OCI_Object  *obj,
    const mtext *attr,
    int          type
)
{
    int res = -1;
    ub2 i;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, -1);
    OCI_CHECK_PTR(OCI_IPC_STRING, attr, -1);

    for (i = 0; i < obj->typinf->nb_cols; i++)
    {
        OCI_Column *col = &obj->typinf->cols[i];

        if (((type == -1) || (col->type == type))  &&
            (mtscasecmp(col->name, attr) == 0))
        {
            res = (int) i;
            break;
        }
    }

    if (res == -1)
        OCI_ExceptionAttributeNotFound(obj->con, attr);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetAttr
 * --------------------------------------------------------------------------------------------- */

void * OCI_ObjectGetAttr
(
    OCI_Object  *obj,
    unsigned int index,
    OCIInd     **pind
)
{
    size_t offset = 0;

    if (obj->typinf->struct_size == 0)
    {
        OCI_ObjectGetStructSize(obj->typinf);
    }

    offset = (size_t) obj->typinf->offsets[index];

    if (pind != NULL)
    {
        int ind_index = obj->idx_ind + OCI_ObjectGetIndOffset(obj->typinf, index);

        *pind = &obj->tab_ind[ind_index];
    }

    return ((char *) obj->handle + offset);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetNumber
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ObjectSetNumber
(
    OCI_Object  *obj,
    const mtext *attr,
    void        *value,
    uword        size,
    uword        flag
)
{
    boolean res = FALSE;
    int index   = 0;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_NUMERIC);

    if (index >= 0)
    {
        OCIInd *ind    = NULL;
        OCINumber *num = OCI_ObjectGetAttr(obj, index, &ind);

        res = OCI_NumberSet(obj->con, num, value, size, flag);

        if (res == TRUE)
            *ind = OCI_IND_NOTNULL;
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetNumber
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ObjectGetNumber
(
    OCI_Object  *obj,
    const mtext *attr,
    void        *value,
    uword        size,
    uword        flag
)
{
    boolean res = FALSE;
    int index   = 0;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_NUMERIC);

    if (index >= 0)
    {
        OCIInd *ind    = NULL;
        OCINumber *num = NULL;

        num = OCI_ObjectGetAttr(obj, index, &ind);

        if ((num != NULL) && (*ind != OCI_IND_NULL))
        {
            res = OCI_NumberGet(obj->con, num, value, size, flag);
        }
    }
    else
    {
        index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TEXT);

        if (index >= 0)
        {
            const mtext *fmt = OCI_GetDefaultFormatNumeric(obj->con);
            ub4 fmt_size     = (ub4) mtslen(fmt);
            dtext *data      = (dtext *) OCI_ObjectGetString(obj, attr);

            res = OCI_NumberGetFromStr(obj->con, value, size, flag, data,
                                       (int) dtslen(data),  fmt, fmt_size);
        }
    }

    OCI_RESULT(res);

    return res;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OCI_API OCI_ObjectCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf
)
{
    OCI_Object *obj = NULL;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf, NULL);

    obj = OCI_ObjectInit(con, &obj, NULL, typinf, NULL, -1, TRUE);

    OCI_RESULT(obj != NULL);

    return obj;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectFree
(
    OCI_Object *obj
)
{
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);

    OCI_CHECK_OBJECT_FETCHED(obj, FALSE);

    /* if the object has sub-objects that have been fetched, we need to free
       these objects */

    OCI_ObjectReset(obj);

    if (obj->objs != NULL)
    {
        OCI_FREE(obj->objs);
    }

    if ((obj->hstate == OCI_OBJECT_ALLOCATED      ) ||
        (obj->hstate == OCI_OBJECT_ALLOCATED_ARRAY))
    {
        OCI_OCIObjectFree(OCILib.env, obj->con->err,  obj->handle,
                          OCI_OBJECTFREE_NONULL);
    }

    OCI_FREE(obj->buf);

    if (obj->hstate != OCI_OBJECT_ALLOCATED_ARRAY)
    {
        OCI_FREE(obj);
    }

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Object ** OCI_API OCI_ObjectArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
)
{
    OCI_Array *arr    = NULL;
    OCI_Object **objs = NULL;

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_OBJECT, 0,
                          sizeof(void *), sizeof(OCI_Object),
                          0, typinf);

    if (arr != NULL)
    {
        objs = (OCI_Object **) arr->tab_obj;
    }

    return objs;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectArrayFree
(
    OCI_Object **objs
)
{
    return OCI_ArrayFreeFromHandles((void **) objs);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectAssign
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectAssign
(
    OCI_Object *obj,
    OCI_Object *obj_src
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj_src, FALSE);

    OCI_CHECK_COMPAT(obj->con, obj->typinf->tdo == obj_src->typinf->tdo, FALSE);

    OCI_CALL2
    (
        res, obj->con,

        OCIObjectCopy(OCILib.env, obj->con->err, obj->con->cxt,
                      obj_src->handle, (obj_src->tab_ind + obj_src->idx_ind),
                      obj->handle, (obj->tab_ind + obj->idx_ind),
                      obj->typinf->tdo, OCI_DURATION_SESSION, OCI_DEFAULT)
    )

    if (res == TRUE)
    {
        obj->typinf = obj_src->typinf;

        OCI_ObjectReset(obj);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetInt
 * --------------------------------------------------------------------------------------------- */

short OCI_API OCI_ObjectGetShort
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    short value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_SHORT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

unsigned short OCI_API OCI_ObjectGetUnsignedShort
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    unsigned short value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_USHORT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetInt
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ObjectGetInt
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    int value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_INT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ObjectGetUnsignedInt
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    unsigned int value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_UINT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetBigInt
 * --------------------------------------------------------------------------------------------- */

big_int OCI_API OCI_ObjectGetBigInt
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    big_int value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_BIGINT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_ObjectGetUnsignedBigInt
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    big_uint value = 0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_BIGUINT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetDouble
 * --------------------------------------------------------------------------------------------- */

double OCI_API OCI_ObjectGetDouble
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    double value = 0.0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_DOUBLE);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetString
 * --------------------------------------------------------------------------------------------- */

const dtext * OCI_API OCI_ObjectGetString
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    const dtext *str = NULL;
    boolean res      = TRUE;
    int index        = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TEXT);

    if (index >= 0)
    {
        OCIInd *ind       = NULL;
        OCIString **value = NULL;

        value = OCI_ObjectGetAttr(obj, index, &ind);

        if ((value != NULL) && (*ind != OCI_IND_NULL))
        {
            str = (dtext *) OCI_StringFromStringPtr(*value, &obj->buf, &obj->buflen);
        }
    }

    OCI_RESULT(res);

    return str;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetRaw
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ObjectGetRaw
(
    OCI_Object  *obj,
    const mtext *attr,
    void        *buffer,
    unsigned int len
)
{
    boolean res = TRUE;
    int index   = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_RAW);
    ub4 raw_len = 0;

    if (index >= 0)
    {
        OCIInd *ind    = NULL;
        OCIRaw **value = NULL;

        value = OCI_ObjectGetAttr(obj, index, &ind);

        if ((value != NULL) && (*ind != OCI_IND_NULL))
        {
            raw_len = OCIRawSize(OCILib.env, *value);

            if (len > raw_len)
                len = raw_len;

            memcpy(buffer, OCIRawPtr(OCILib.env, *value), (size_t) len);
        }
    }

    OCI_RESULT(res);

    return len;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OCI_API OCI_ObjectGetDate
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    OCI_Date * date = NULL;
    boolean res     = TRUE;
    int index       = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_DATETIME);

    if (index >= 0)
    {
        OCIInd *ind    = NULL;
        OCIDate *value = NULL;

        value = OCI_ObjectGetAttr(obj, index, &ind);

        if ((value != NULL) && (*ind != OCI_IND_NULL))
        {
            date = OCI_DateInit(obj->con, (OCI_Date **) &obj->objs[index],
                                value, FALSE, FALSE);

            res = (date != NULL);
        }
    }

    OCI_RESULT(res);

    return date;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_API OCI_ObjectGetTimestamp
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    OCI_Timestamp *tmsp = NULL;
    boolean res         = TRUE;

    int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TIMESTAMP);

    if (index >= 0)
    {
        OCIInd *ind         = NULL;
        OCIDateTime **value = NULL;

        value = OCI_ObjectGetAttr(obj, index, &ind);

        if ((value != NULL) && (*ind != OCI_IND_NULL))
        {
            tmsp = OCI_TimestampInit(obj->con,
                                     (OCI_Timestamp **) &obj->objs[index],
                                     (OCIDateTime *) *value,
                                     obj->typinf->cols[index].subtype);

            res = (tmsp != NULL);
        }
    }

    OCI_RESULT(res);

    return tmsp;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OCI_API OCI_ObjectGetInterval
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    OCI_Interval *itv = NULL;
    boolean res       = TRUE;
    int index         = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_INTERVAL);

    if (index >= 0)
    {
        OCIInd *ind         = NULL;
        OCIInterval **value = NULL;

        value = OCI_ObjectGetAttr(obj, index, &ind);

        if ((value != NULL) && (*ind != OCI_IND_NULL))
        {
            itv = OCI_IntervalInit(obj->con,
                                   (OCI_Interval **) &obj->objs[index],
                                   (OCIInterval *) *value,
                                   obj->typinf->cols[index].subtype);

            res = (itv != NULL);
        }
    }

    OCI_RESULT(res);

    return itv;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetColl
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_API OCI_ObjectGetColl
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    OCI_Coll *coll = NULL;
    boolean res    = TRUE;
    int index      = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_COLLECTION);

    if (index >= 0)
    {
        OCIInd *ind     = NULL;
        OCIColl **value = NULL;

        value = OCI_ObjectGetAttr(obj, index, &ind);

        if ((value != NULL) && (*ind != OCI_IND_NULL))
        {
            coll = OCI_CollInit(obj->con,
                                (OCI_Coll **) &obj->objs[index],
                                (OCIColl *) *value,
                                obj->typinf->cols[index].typinf);

            res = (coll != NULL);
        }
    }

    OCI_RESULT(res);

    return coll;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OCI_API OCI_ObjectGetObject
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    OCI_Object *obj2 = NULL;
    boolean res      = TRUE;
    int index        = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_OBJECT);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        void *value = NULL;

        value = OCI_ObjectGetAttr(obj, index, &ind);

        if ((value != NULL) && (*ind != OCI_IND_NULL))
        {
            obj2 = OCI_ObjectInit(obj->con, (OCI_Object **) &obj->objs[index],
                                  value, obj->typinf->cols[index].typinf,
                                  obj, index, FALSE);

            res = (obj2 != NULL);
        }
    }

    OCI_RESULT(res);

    return obj2;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OCI_API OCI_ObjectGetLob
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    OCI_Lob *lob = NULL;
    boolean res  = TRUE;
    int index    = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_LOB);

    if (index >= 0)
    {
        OCIInd *ind           = NULL;
        OCILobLocator **value = NULL;

        value = OCI_ObjectGetAttr(obj, index, &ind);

        if ((value != NULL) && (*ind != OCI_IND_NULL))
        {
            lob = OCI_LobInit(obj->con, (OCI_Lob **) &obj->objs[index],
                              *value, obj->typinf->cols[index].subtype);

            res = (lob != NULL);
        }
    }

    OCI_RESULT(res);

    return lob;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * OCI_API OCI_ObjectGetFile
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    OCI_File *file = NULL;
    boolean res    = TRUE;
    int index      = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_FILE);

    if (index >= 0)
    {
        OCIInd *ind           = NULL;
        OCILobLocator **value = NULL;

        value = OCI_ObjectGetAttr(obj, index, &ind);

        if ((value != NULL) && (*ind != OCI_IND_NULL))
        {
            file = OCI_FileInit(obj->con, (OCI_File **) &obj->objs[index],
                                *value, obj->typinf->cols[index].subtype);

            res = (file != NULL);
        }
    }

    OCI_RESULT(res);

    return file;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetRef
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OCI_API OCI_ObjectGetRef
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    OCI_Ref *ref = NULL;
    boolean res  = TRUE;
    int index    = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_REF);

    if (index >= 0)
    {
        OCIInd *ind    = NULL;
        OCIRef **value = NULL;

        value = OCI_ObjectGetAttr(obj, index, &ind);

        if ((value != NULL) && (*ind != OCI_IND_NULL))
        {
            ref = OCI_RefInit(obj->con, NULL, (OCI_Ref **) &obj->objs[index],
                              *value);

            res = (ref != NULL);
        }
    }

    OCI_RESULT(res);

    return ref;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetShort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetShort
(
    OCI_Object  *obj,
    const mtext *attr,
    short        value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_SHORT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetUnsignedShort
(
    OCI_Object    *obj,
    const mtext   *attr,
    unsigned short value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_USHORT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetInt
(
    OCI_Object  *obj,
    const mtext *attr,
    int          value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_INT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetUnsignedInt
(
    OCI_Object  *obj,
    const mtext *attr,
    unsigned int value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_UINT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetBigInt
(
    OCI_Object  *obj,
    const mtext *attr,
    big_int      value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_BIGINT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetUnsignedBigInt
(
    OCI_Object  *obj,
    const mtext *attr,
    big_uint     value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_BIGUINT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetDouble
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetDouble
(
    OCI_Object  *obj,
    const mtext *attr,
    double       value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value),
                               (uword) OCI_NUM_DOUBLE);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetString
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetString
(
    OCI_Object  *obj,
    const mtext *attr,
    const dtext *value
)
{
    boolean res = TRUE;

    if (value == NULL)
    {
        res = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TEXT);

        if (index >= 0)
        {
            OCIInd *ind      = NULL;
            OCIString **data = OCI_ObjectGetAttr(obj, index, &ind);

            res = OCI_StringToStringPtr(data, obj->con->err, (void *) value,
                                        &obj->buf, &obj->buflen);

            if (res == TRUE)
                *ind = OCI_IND_NOTNULL;
        }
        else
            res = FALSE;
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetRaw
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetRaw
(
    OCI_Object  *obj,
    const mtext *attr,
    void       * value,
    unsigned int len
)
{
    boolean res = TRUE;

    if (value == NULL)
    {
        res = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        int index;

        OCI_CHECK_MIN(obj->con, NULL, len, 1, FALSE);

        index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_RAW);

        if (index >= 0)
        {
            OCIInd *ind   = NULL;
            OCIRaw **data = OCI_ObjectGetAttr(obj, index, &ind);

            OCI_CALL2
            (
                res, obj->con,

                OCIRawAssignBytes(OCILib.env, obj->con->err, (ub1*) value,
                                  len, data)
            )

            if (res == TRUE)
                *ind = OCI_IND_NOTNULL;
        }
        else
            res = FALSE;
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetDate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetDate
(
    OCI_Object  *obj,
    const mtext *attr,
    OCI_Date    *value
)
{
    boolean res = TRUE;

    if (value == NULL)
    {
        res = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_DATETIME);

        if (index >= 0)
        {
            OCIInd * ind  = NULL;
            OCIDate *data = OCI_ObjectGetAttr(obj, index, &ind);

            OCI_CALL2
            (
                res, obj->con,

                OCIDateAssign(obj->con->err, value->handle, data)
            )

            if (res == TRUE)
                *ind = OCI_IND_NOTNULL;
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetTimestamp
(
    OCI_Object    *obj,
    const mtext   *attr,
    OCI_Timestamp *value
)
{
    boolean res = TRUE;

    if (value == NULL)
    {
        res = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TIMESTAMP);

        if (index >= 0)
        {
            #if OCI_VERSION_COMPILE >= OCI_9_0

            OCIInd * ind       = NULL;
            OCIDateTime **data = OCI_ObjectGetAttr(obj, index, &ind);

            OCI_CALL2
            (
                res, obj->con,

                OCIDateTimeAssign((dvoid *) OCILib.env, obj->con->err,
                                  value->handle, *data)
            )

            if (res == TRUE)
                *ind = OCI_IND_NOTNULL;

            #endif
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetInterval
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetInterval
(
    OCI_Object   *obj,
    const mtext  *attr,
    OCI_Interval *value
)
{
    boolean res = TRUE;

    if (value == NULL)
    {
        res = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_INTERVAL);

        if (index >= 0)
        {
            #if OCI_VERSION_COMPILE >= OCI_9_0

            OCIInd * ind       = NULL;
            OCIInterval **data = OCI_ObjectGetAttr(obj, index, &ind);

            OCI_CALL2
            (
                res, obj->con,

                OCIIntervalAssign((dvoid *) OCILib.env, obj->con->err,
                                  value->handle, *data)
            )

            if (res == TRUE)
                *ind = OCI_IND_NOTNULL;

            #endif
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetColl
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetColl
(
    OCI_Object  *obj,
    const mtext *attr,
    OCI_Coll    *value
)
{
    boolean res = TRUE;

    if (value == NULL)
    {
        res = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_COLLECTION);

        if (index >= 0)
        {
            OCIInd *ind    = NULL;
            OCIColl **data = OCI_ObjectGetAttr(obj, index, &ind);

            OCI_CALL2
            (
                res, obj->con,

                OCICollAssign(OCILib.env, obj->con->err, value->handle, *data)
            )

            if (res == TRUE)
                *ind = OCI_IND_NOTNULL;
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetObject
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetObject
(
    OCI_Object  *obj,
    const mtext *attr,
    OCI_Object  *value
)
{
    boolean res = TRUE;

    if (value == NULL)
    {
        res = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_OBJECT);

        if (index >= 0)
        {
            OCIInd *ind = NULL;
            void *data  = OCI_ObjectGetAttr(obj, index, &ind);

            OCI_CALL2
            (
                res, obj->con,

                OCIObjectCopy(OCILib.env, obj->con->err, obj->con->cxt,
                              value->handle, (value->tab_ind + value->idx_ind),
                              data, ind, obj->typinf->cols[index].typinf->tdo,
                              OCI_DURATION_SESSION, OCI_DEFAULT)
            )

            if (res == TRUE)
                *ind = OCI_IND_NOTNULL;
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetLob
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetLob
(
    OCI_Object  *obj,
    const mtext *attr,
    OCI_Lob     *value
)
{
    boolean res = TRUE;

    if (value == NULL)
    {
        res = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_LOB);

        if (index >= 0)
        {
            OCIInd * ind = NULL;
            void **data  = OCI_ObjectGetAttr(obj, index, &ind);

            OCI_CALL2
            (
                res, obj->con,

                OCILobLocatorAssign(obj->con->cxt, obj->con->err,
                                    value->handle, (OCILobLocator **) data)
            )

            if (res == TRUE)
                *ind = OCI_IND_NOTNULL;
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetFile
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetFile
(
    OCI_Object  *obj,
    const mtext *attr,
    OCI_File    *value
)
{
    boolean res = TRUE;

    if (value == NULL)
    {
        res = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_FILE);

        if (index >= 0)
        {
            OCIInd *ind = NULL;
            void **data = OCI_ObjectGetAttr(obj, index, &ind);

            OCI_CALL2
            (
                res, obj->con,

                OCILobLocatorAssign(obj->con->cxt, obj->con->err,
                                    value->handle, (OCILobLocator **) data)
            )

            if (res == TRUE)
                *ind = OCI_IND_NOTNULL;
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetRef
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetRef
(
    OCI_Object  *obj,
    const mtext *attr,
    OCI_Ref     *value
)
{
    boolean res = FALSE;

    if (value == NULL)
    {
        res = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_REF);

        if (index >= 0)
        {
            OCIInd *ind   = NULL;
            OCIRef **data = OCI_ObjectGetAttr(obj, index, &ind);

            OCI_CALL2
            (
                res, obj->con,

                OCIRefAssign(OCILib.env, obj->con->err, value->handle, data)
            )

            if (res == TRUE)
                *ind = OCI_IND_NOTNULL;
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetNull
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    boolean res = TRUE;
    int index;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, attr, FALSE);

    index = OCI_ObjectGetAttrIndex(obj, attr, -1);

    if (index >= 0)
    {
        int ind_index = obj->idx_ind + OCI_ObjectGetIndOffset(obj->typinf, index);

        obj->tab_ind[ind_index] = OCI_IND_NULL;

        res = TRUE;
    }
    else
        res = FALSE;

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectIsNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectIsNull
(
    OCI_Object  *obj,
    const mtext *attr
)
{
    boolean res = TRUE;
    boolean ret = TRUE;
    int index   = 0;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, attr, FALSE);

    index = OCI_ObjectGetAttrIndex(obj, attr, -1);

    if (index >= 0)
    {
        int ind_index = obj->idx_ind + OCI_ObjectGetIndOffset(obj->typinf, index);

        ret = (obj->tab_ind[ind_index] != OCI_IND_NOTNULL);

        res = TRUE;
    }
    else
        res = FALSE;

    OCI_RESULT(res);

    return ret;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OCI_API OCI_ObjectGetTypeInfo
(
    OCI_Object *obj
)
{
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, NULL);

    OCI_RESULT(TRUE);

    return obj->typinf;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ObjectGetType
(
    OCI_Object *obj
)
{
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return (unsigned int) obj->type;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetSelfRef
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectGetSelfRef
(
    OCI_Object *obj,
    OCI_Ref    *ref
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);
    OCI_CHECK_PTR(OCI_IPC_REF, ref, FALSE);

    OCI_CHECK_COMPAT(obj->con, obj->typinf->tdo == ref->typinf->tdo, FALSE);

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

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetStruct
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectGetStruct
(
    OCI_Object *obj,
    void      **pp_struct,
    void     ** pp_ind
)
{
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);

    OCI_RESULT(TRUE);

    *pp_struct = (void *) obj->handle;

    if (pp_ind)
        *pp_ind = (void *) obj->tab_ind;

    OCI_RESULT(TRUE);

    return TRUE;
}

