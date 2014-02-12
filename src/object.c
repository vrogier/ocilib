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
 * $Id: object.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

#define OCI_OBJECT_SET_VALUE(datatype, type, func)                          \
    if (!value)                                                             \
    {                                                                       \
        res = OCI_ObjectSetNull(obj, attr);                                 \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        int index = OCI_ObjectGetAttrIndex(obj, attr, datatype);            \
                                                                            \
        if (index >= 0)                                                     \
        {                                                                   \
            OCIInd  *ind  = NULL;                                           \
            type    *data = (type *)  OCI_ObjectGetAttr(obj, index, &ind);  \
                                                                            \
            res = TRUE;                                                     \
                                                                            \
            OCI_CALL2(res, obj->con, func)                                  \
                                                                            \
            if (res)                                                        \
            {                                                               \
                *ind = OCI_IND_NOTNULL;                                     \
            }                                                               \
        }                                                                   \
    }                                                                       \

#define OCI_OBJECT_GET_VALUE(ret, datatype, type, func)                     \
                                                                            \
    int index = OCI_ObjectGetAttrIndex(obj, attr, datatype);                \
                                                                            \
    if (index >= 0)                                                         \
    {                                                                       \
        OCIInd *ind   = NULL;                                               \
        type   *value = NULL;                                               \
                                                                            \
        res = TRUE;                                                         \
                                                                            \
        value = (type *) OCI_ObjectGetAttr(obj, index, &ind);               \
                                                                            \
        if (value && ind && (OCI_IND_NULL != *ind))                         \
        {                                                                   \
            ret = func;                                                     \
                                                                            \
            res = (ret != NULL);                                            \
        }                                                                   \
    }                                                                       \

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
        if (OCI_CDT_OBJECT == typinf->cols[i].datatype)
        {
            j += OCI_ObjectGetIndOffset(typinf->cols[i].typinf, typinf->cols[i].typinf->nb_cols);
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
    size_t size = 0;

    if (typinf->struct_size != 0)
    {
        size = typinf->struct_size;
    }
    else
    {
        size_t size1 = 0;
        size_t size2 = 0;

        int type1 = 0;
        int type2 = 0;

        boolean align = FALSE;
        ub2 i;

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
                {
                    align = TRUE;
                    break;
                }
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
 * OCI_ObjectGetUserStructSize
 * --------------------------------------------------------------------------------------------- */

size_t OCI_ObjectGetUserStructSize
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

    for (i = 0; i < typinf->nb_cols; i++)
    {
        align = FALSE;

        OCI_ColumnGetAttrInfo(&typinf->cols[i],   typinf->nb_cols, i, &size1, &type1);
        OCI_ColumnGetAttrInfo(&typinf->cols[i+1], typinf->nb_cols, i, &size2, &type2);

        switch (OCI_OFFSET_PAIR(type1, type2))
        {
            case OCI_OFFSET_PAIR(OCI_OFT_NUMBER, OCI_OFT_POINTER):
            case OCI_OFFSET_PAIR(OCI_OFT_DATE,   OCI_OFT_POINTER):
            case OCI_OFFSET_PAIR(OCI_OFT_OBJECT, OCI_OFT_POINTER):
            case OCI_OFFSET_PAIR(OCI_OFT_NUMBER, OCI_OFT_STRUCT):
            case OCI_OFFSET_PAIR(OCI_OFT_DATE,   OCI_OFT_STRUCT):
            case OCI_OFFSET_PAIR(OCI_OFT_OBJECT, OCI_OFT_STRUCT): 
            case OCI_OFFSET_PAIR(OCI_OFT_STRUCT, OCI_OFT_STRUCT): 
            {
                align = TRUE;
                break;
            }
        }

        size += size1;

        if (align)
        {
            size = ROUNDUP(size, OCI_DEF_ALIGN);
        }
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

    switch (typinf->cols[index].datatype)
    {
        case OCI_CDT_NUMERIC:
        {
            ub4 subtype = typinf->cols[index].subtype;

            if (subtype & OCI_NUM_FLOAT)
            {
                *p_size = sizeof(double);
                *p_type = OCI_OFT_FLOAT;
            }
            else if (subtype & OCI_NUM_DOUBLE)
            {
                *p_size = sizeof(double);
                *p_type = OCI_OFT_DOUBLE;
            }
            else
            {
                *p_size = sizeof(OCINumber);
                *p_type = OCI_OFT_NUMBER;
            }
            
            break;
        }
        case OCI_CDT_DATETIME:
        {
            *p_size = sizeof(OCIDate);
            *p_type = OCI_OFT_DATE;
            break;
        }
        case OCI_CDT_OBJECT:
        {
            *p_size = OCI_ObjectGetStructSize(typinf->cols[index].typinf);
            *p_type = OCI_OFT_OBJECT;
            break;
        }
        default:
        {
            *p_size = sizeof(void *);
            *p_type = OCI_OFT_POINTER;
            break;
        }
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
    boolean      res = TRUE;

    OCI_CHECK(pobj == NULL, NULL);

    if (!*pobj)
    {
        *pobj = (OCI_Object *) OCI_MemAlloc(OCI_IPC_OBJECT, sizeof(*obj), (size_t) 1, TRUE);
    }

    if (*pobj)
    {
        obj = *pobj;

        obj->con    = con;
        obj->handle = handle;
        obj->typinf = typinf;

        if (!obj->objs)
        {
            obj->objs = (void **) OCI_MemAlloc(OCI_IPC_BUFF_ARRAY, sizeof(void *),
                                               (size_t) typinf->nb_cols, TRUE);
        }
        else
        {
            OCI_ObjectReset(obj);
        }

        res = (obj->objs != NULL);

        if (res && (!obj->handle || (OCI_OBJECT_ALLOCATED_ARRAY == obj->hstate)))
        {
            /* allocates handle for non fetched object */

            if (OCI_OBJECT_ALLOCATED_ARRAY != obj->hstate)
            {
                obj->hstate = OCI_OBJECT_ALLOCATED;
            }

            OCI_CALL2
            (
                res, obj->con,

                OCI_ObjectNew(obj->con->env,  obj->con->err, obj->con->cxt,
                              (OCITypeCode) SQLT_NTY, obj->typinf->tdo, (dvoid *) NULL,
                              (OCIDuration) OCI_DURATION_SESSION, (boolean) TRUE,
                              (dvoid **) &obj->handle)
            )
        }
        else
        {
            obj->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }

        if (res && (OCI_UNKNOWN == obj->type))
        {
            ub4 size = sizeof(obj->type);

            /* calling OCIObjectGetProperty() on objects that are attributes of
               parent objects leads to a segfault on MS Windows !
               We need to report that to Oracle! Because sub objects always are
               values, if the parent indicator array is not null, let's assign
               the object type properties ourselves */

            if (!parent)
            {
                OCIObjectGetProperty(obj->con->env, obj->con->err, obj->handle,
                                     (OCIObjectPropId) OCI_OBJECTPROP_LIFETIME,
                                     (void *) &obj->type, &size);
            }
            else
            {
                obj->type = OCI_OBJECT_VALUE;
            }
        }

        if (res && (reset || !obj->tab_ind))
        {
            if (!parent)
            {
                OCI_CALL2
                (
                    res, obj->con,

                    OCIObjectGetInd(obj->con->env, obj->con->err,
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
    {
        res = FALSE;
    }

    /* check for failure */

    if (!res)
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

            if (OCI_OBJECT_FETCHED_CLEAN == data->hstate)
            {
                data->hstate =  OCI_OBJECT_FETCHED_DIRTY;
            }

            switch (obj->typinf->cols[i].datatype)
            {
                case OCI_CDT_DATETIME:
                {
                    OCI_DateFree((OCI_Date *) obj->objs[i]);
                    break;
                }
                case OCI_CDT_LOB:
                {
                    OCI_LobFree((OCI_Lob *) obj->objs[i]);
                    break;
                }
                case OCI_CDT_FILE:
                {
                    OCI_FileFree((OCI_File *) obj->objs[i]);
                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    OCI_ObjectFree((OCI_Object *) obj->objs[i]);
                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    OCI_CollFree((OCI_Coll *) obj->objs[i]);
                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    OCI_TimestampFree((OCI_Timestamp *) obj->objs[i]);
                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    OCI_IntervalFree((OCI_Interval *) obj->objs[i]);
                    break;
                }
                case OCI_CDT_REF:
                {
                    OCI_RefFree((OCI_Ref *) obj->objs[i]);
                    break;
                }
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
    const otext *attr,
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

        if (((type == -1) || (col->datatype == type))  && (ostrcasecmp(col->name, attr) == 0))
        {
            res = (int) i;
            break;
        }
    }

    if (res == -1)
    {
        OCI_ExceptionAttributeNotFound(obj->con, attr);
    }

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

    if (pind)
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
    const otext *attr,
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
        OCIInd *ind = NULL;
        void   *num = OCI_ObjectGetAttr(obj, index, &ind);

        res = OCI_NumberSet(obj->con, num, size, flag, obj->typinf->cols[index].libcode, value);

        if (res)
        {
            *ind = OCI_IND_NOTNULL;
        }
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
    const otext *attr,
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
        OCIInd *ind = NULL;
        void   *num = NULL;

        num = OCI_ObjectGetAttr(obj, index, &ind);

        if (num && (OCI_IND_NULL != *ind))
        {
            res = OCI_NumberGet(obj->con, num, size, flag, obj->typinf->cols[index].libcode, value);
        }
    }
    else
    {
        index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TEXT);

        if (index >= 0)
        {          
            res = OCI_NumberFromString(obj->con, value, size, flag, obj->typinf->cols[index].libcode, 
                                       OCI_ObjectGetString(obj, attr), NULL);
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

    if (obj->objs)
    {
        OCI_FREE(obj->objs);
    }

    if ((OCI_OBJECT_ALLOCATED == obj->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == obj->hstate))
    {
        OCI_OCIObjectFree(obj->con->env, obj->con->err,  obj->handle, OCI_OBJECTFREE_NONULL);
    }

    OCI_FREE(obj->tmpbuf);

    if (OCI_OBJECT_ALLOCATED_ARRAY != obj->hstate)
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

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_OBJECT, 0, sizeof(void *), sizeof(OCI_Object), 0, typinf);

    if (arr)
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

        OCIObjectCopy(obj->con->env, obj->con->err, obj->con->cxt,
                      obj_src->handle, (obj_src->tab_ind + obj_src->idx_ind),
                      obj->handle, (obj->tab_ind + obj->idx_ind),
                      obj->typinf->tdo, OCI_DURATION_SESSION, OCI_DEFAULT)
    )

    if (res)
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
    const otext *attr
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
    const otext *attr
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
    const otext *attr
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
    const otext *attr
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
    const otext *attr
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
    const otext *attr
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
    const otext *attr
)
{
    double value = 0.0;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_DOUBLE);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetFloat
 * --------------------------------------------------------------------------------------------- */

float OCI_API OCI_ObjectGetFloat
(
    OCI_Object  *obj,
    const otext *attr
)
{
    float value = 0.0f;

    OCI_ObjectGetNumber(obj, attr, &value, sizeof(value), OCI_NUM_FLOAT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_ObjectGetString
(
    OCI_Object  *obj,
    const otext *attr
)
{
    boolean res = FALSE;

    const otext *str = NULL;
    int index        = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TEXT);

    if (index >= 0)
    {
        OCIInd *ind       = NULL;
        OCIString **value = NULL;

        res = TRUE;

        value = (OCIString **) OCI_ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            str = OCI_StringFromStringPtr(obj->con->env, *value,  &obj->tmpbuf, &obj->tmpsize);            
        }
    }
    else
    {
        index = OCI_ObjectGetAttrIndex(obj, attr, -1);

        if (index >= 0)
        {
            OCIInd      *ind   = NULL;
            void        *value = NULL;
            unsigned int size  = 0;
            unsigned int len   = 0;

            res = TRUE;

            value = OCI_ObjectGetAttr(obj, index, &ind);

            /* special case of RAW attribute, we need their length */
            if (OCI_CDT_RAW == obj->typinf->cols[index].datatype)
            {
                if (value && ind && (OCI_IND_NULL != *ind))
                {
                    size  = OCIRawSize(obj->con->env, (*(OCIRaw **) value));
                    value = OCIRawPtr(obj->con->env,  (*(OCIRaw **) value));
                }
            }

            len = OCI_StringGetFromType(obj->con, &obj->typinf->cols[index], value, size, NULL, FALSE); 

            if (len > 0)
            {
                res = OCI_StringRequestBuffer(&obj->tmpbuf, &obj->tmpsize, len);

                if (res)
                {
                    len = OCI_StringGetFromType(obj->con, &obj->typinf->cols[index], value, size, obj->tmpbuf, FALSE);                 
                }
            }
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
    const otext *attr,
    void        *buffer,
    unsigned int len
)
{
    boolean res = FALSE;

    int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_RAW);

    if (index >= 0)
    {
        OCIInd *ind    = NULL;
        OCIRaw **value = NULL;

        res = TRUE;

        value = (OCIRaw **) OCI_ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            ub4 raw_len = OCIRawSize(obj->con->env, *value);

            if (len > raw_len)
            {
                len = raw_len;
            }

            memcpy(buffer, OCIRawPtr(obj->con->env, *value), (size_t) len);
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
    const otext *attr
)
{
    OCI_Date *date = NULL;
    boolean   res  = FALSE;

    OCI_OBJECT_GET_VALUE
    (
        date, 
        OCI_CDT_DATETIME,
        OCIDate,
        OCI_DateInit(obj->con, (OCI_Date **) &obj->objs[index], value, FALSE, FALSE)
    )
 
    OCI_RESULT(res);

    return date;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_API OCI_ObjectGetTimestamp
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OCI_Timestamp *tmsp = NULL;
    boolean        res  = FALSE;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_OBJECT_GET_VALUE
    (
        tmsp, 
        OCI_CDT_TIMESTAMP,
        OCI_Timestamp *,
        OCI_TimestampInit(obj->con, (OCI_Timestamp **) &obj->objs[index],
                          (OCIDateTime *) *value, obj->typinf->cols[index].subtype)
    )
 
#endif

    OCI_RESULT(res);

    return tmsp;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OCI_API OCI_ObjectGetInterval
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OCI_Interval *itv = NULL;
    boolean       res = FALSE;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_OBJECT_GET_VALUE
    (
        itv, 
        OCI_CDT_INTERVAL,
        OCIInterval *,
        OCI_IntervalInit(obj->con, (OCI_Interval **) &obj->objs[index],
                         (OCIInterval *) *value, obj->typinf->cols[index].subtype)
    )
 
#endif

    OCI_RESULT(res);

    return itv;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetColl
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_API OCI_ObjectGetColl
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OCI_Coll *coll = NULL;
    boolean   res  = FALSE;

    OCI_OBJECT_GET_VALUE
    (
        coll, 
        OCI_CDT_COLLECTION,
        OCI_Coll *,
        OCI_CollInit(obj->con, (OCI_Coll **) &obj->objs[index],
                     (OCIColl *) *value, obj->typinf->cols[index].typinf)
    )
 
    OCI_RESULT(res);

    return coll;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OCI_API OCI_ObjectGetObject
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OCI_Object *obj2 = NULL;
    boolean     res  = FALSE;

    OCI_OBJECT_GET_VALUE
    (
        obj2, 
        OCI_CDT_OBJECT,
        void,
        OCI_ObjectInit(obj->con, (OCI_Object **) &obj->objs[index], value,
                       obj->typinf->cols[index].typinf,  obj, index, FALSE)
    )
 
    OCI_RESULT(res);

    return obj2;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OCI_API OCI_ObjectGetLob
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OCI_Lob *lob = NULL;
    boolean  res = FALSE;

    OCI_OBJECT_GET_VALUE
    (
        lob, 
        OCI_CDT_LOB,
        OCILobLocator*,
        OCI_LobInit(obj->con, (OCI_Lob **) &obj->objs[index], *value, obj->typinf->cols[index].subtype)
    )
 
    OCI_RESULT(res);

    return lob;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * OCI_API OCI_ObjectGetFile
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OCI_File *file = NULL;
    boolean   res  = FALSE;

    OCI_OBJECT_GET_VALUE
    (
        file, 
        OCI_CDT_FILE,
        OCILobLocator*,
        OCI_FileInit(obj->con, (OCI_File **) &obj->objs[index], *value, obj->typinf->cols[index].subtype)
    )
 
    OCI_RESULT(res);

    return file;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetRef
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OCI_API OCI_ObjectGetRef
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OCI_Ref *ref = NULL;
    boolean  res = FALSE;

    OCI_OBJECT_GET_VALUE
    (
        ref, 
        OCI_CDT_REF,
        OCIRef*,
        OCI_RefInit(obj->con, NULL, (OCI_Ref **) &obj->objs[index], *value)
    )

    OCI_RESULT(res);

    return ref;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetShort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetShort
(
    OCI_Object  *obj,
    const otext *attr,
    short        value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), (uword) OCI_NUM_SHORT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetUnsignedShort
(
    OCI_Object    *obj,
    const otext   *attr,
    unsigned short value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), (uword) OCI_NUM_USHORT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetInt
(
    OCI_Object  *obj,
    const otext *attr,
    int          value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), (uword) OCI_NUM_INT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetUnsignedInt
(
    OCI_Object  *obj,
    const otext *attr,
    unsigned int value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), (uword) OCI_NUM_UINT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetBigInt
(
    OCI_Object  *obj,
    const otext *attr,
    big_int      value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), (uword) OCI_NUM_BIGINT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetUnsignedBigInt
(
    OCI_Object  *obj,
    const otext *attr,
    big_uint     value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), (uword) OCI_NUM_BIGUINT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetDouble
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetDouble
(
    OCI_Object  *obj,
    const otext *attr,
    double       value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), (uword) OCI_NUM_DOUBLE);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetFloat
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetFloat
(
    OCI_Object  *obj,
    const otext *attr,
    float        value
)
{
    return OCI_ObjectSetNumber(obj, attr, &value, sizeof(value), (uword) OCI_NUM_FLOAT);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetString
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetString
(
    OCI_Object  *obj,
    const otext *attr,
    const otext *value
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
            OCIString **data = (OCIString **) OCI_ObjectGetAttr(obj, index, &ind);
           
            res = OCI_StringToStringPtr(obj->con->env, data, obj->con->err, value);

            if (res == TRUE)
            {
                *ind = OCI_IND_NOTNULL;
            }
        }
        else
        {
            res = FALSE;
        }
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
    const otext *attr,
    void       * value,
    unsigned int len
)
{
    boolean res = FALSE;

    if (value)
    {
        OCI_CHECK_MIN(obj->con, NULL, len, 1, FALSE);
    }

    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_RAW, 
        OCIRaw*, 
        OCIRawAssignBytes(obj->con->env, obj->con->err, (ub1*) value, len, data)
    )
    
    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetDate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetDate
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Date    *value
)
{
    boolean res = FALSE;

    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_DATETIME, 
        OCIDate, 
        OCIDateAssign(obj->con->err, value->handle, data)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetTimestamp
(
    OCI_Object    *obj,
    const otext   *attr,
    OCI_Timestamp *value
)
{
    boolean res = FALSE;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_TIMESTAMP, 
        OCIDateTime*, 
        OCIDateTimeAssign((dvoid *) obj->con->env, obj->con->err, value->handle, *data)
    )

#endif

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetInterval
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetInterval
(
    OCI_Object   *obj,
    const otext  *attr,
    OCI_Interval *value
)
{
    boolean res = FALSE;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_INTERVAL, 
        OCIInterval*, 
        OCIIntervalAssign((dvoid *) obj->con->env, obj->con->err, value->handle, *data)
    )

#endif

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetColl
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetColl
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Coll    *value
)
{
    boolean res = FALSE;

    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_COLLECTION, 
        OCIColl*, 
        OCICollAssign(obj->con->env, obj->con->err, value->handle, *data)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetObject
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetObject
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Object  *value
)
{
    boolean res = FALSE;

    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_OBJECT, 
        void, 
        OCIObjectCopy(obj->con->env, obj->con->err, obj->con->cxt,
                     value->handle, (value->tab_ind + value->idx_ind),
                     data, ind, obj->typinf->cols[index].typinf->tdo,
                     OCI_DURATION_SESSION, OCI_DEFAULT)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetLob
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetLob
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Lob     *value
)
{
    boolean res = FALSE;

    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_LOB, 
        OCILobLocator*, 
        OCILobLocatorAssign(obj->con->cxt, obj->con->err, value->handle, (OCILobLocator **) data)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetFile
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetFile
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_File    *value
)
{
    boolean res = FALSE;

    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_FILE, 
        OCILobLocator*, 
        OCILobLocatorAssign(obj->con->cxt, obj->con->err, value->handle, (OCILobLocator **) data)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetRef
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetRef
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Ref     *value
)
{
    boolean res = FALSE;

    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_REF, 
        OCIRef*, 
        OCIRefAssign(obj->con->env, obj->con->err, value->handle, data)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetNull
(
    OCI_Object  *obj,
    const otext *attr
)
{
    boolean res = FALSE;
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

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectIsNull
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectIsNull
(
    OCI_Object  *obj,
    const otext *attr
)
{
    boolean res = FALSE;
    boolean ret = TRUE;
    int index   = 0;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, attr, FALSE);

    index = OCI_ObjectGetAttrIndex(obj, attr, -1);

    if (index >= 0)
    {
        int ind_index = obj->idx_ind + OCI_ObjectGetIndOffset(obj->typinf, index);

        ret = (OCI_IND_NOTNULL != obj->tab_ind[ind_index]);

        res = TRUE;
    }

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

        OCIObjectGetObjectRef(obj->con->env, obj->con->err, obj->handle, ref->handle)
    )

    if (res && ref->obj)
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
    void      **pp_ind
)
{
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);

    OCI_RESULT(TRUE);

    if (pp_struct)
    {
        *pp_struct = (void *) obj->handle;
    }

    if (pp_ind)
    {
        *pp_ind = (void *) obj->tab_ind;
    }

    OCI_RESULT(TRUE);

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectToText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectToText
(
    OCI_Object   *obj,
    unsigned int *size,
    otext        *str
)
{
    boolean      res   = TRUE;
    otext       *attr  = NULL;
    boolean      quote = TRUE;
    unsigned int len   = 0;

    int i;

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj, FALSE);
    OCI_CHECK_PTR(OCI_IPC_VOID, size, FALSE);

    if (str)
    {
        *str = 0;
    }

    len += OCI_StringAddToBuffer(str, len, obj->typinf->name, FALSE);
    len += OCI_StringAddToBuffer(str, len, OTEXT("("), FALSE);

    for (i = 0; i < obj->typinf->nb_cols; i++)
    {
        attr  = obj->typinf->cols[i].name;
        quote = TRUE;

        if (OCI_ObjectIsNull(obj, attr))
        {
            len += OCI_StringAddToBuffer(str, len, OCI_STRING_NULL, FALSE);
        }
        else
        {
            void        *data      = NULL;
            unsigned int data_size = 0;

            switch (obj->typinf->cols[i].datatype)
            {
                case OCI_CDT_TEXT:
                {
                    data  = (void *) OCI_ObjectGetString(obj, attr);
                    break;
                }
                case OCI_CDT_NUMERIC:
                {
                    OCIInd *ind = NULL;
                    data  = OCI_ObjectGetAttr(obj, i, &ind);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_RAW:
                {
                    OCIInd *ind = NULL;
                    data = OCI_ObjectGetAttr(obj, i, &ind);
                                     
                    if (data && ind && (OCI_IND_NULL != *ind))
                    {
                        data_size = OCIRawSize(obj->con->env, (*(OCIRaw **) data));
                        data      = OCIRawPtr(obj->con->env,  (*(OCIRaw **) data));
                    }
                    break;
                }
                case OCI_CDT_DATETIME:
                {
                    data  = (void *) OCI_ObjectGetDate(obj, attr);
                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    data  = (void *) OCI_ObjectGetTimestamp(obj, attr);
                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    data  = (void *) OCI_ObjectGetInterval(obj, attr);
                    break;
                }
                case OCI_CDT_LOB:
                {
                    data  = (void *) OCI_ObjectGetLob(obj, attr);
                    break;
                }
                case OCI_CDT_FILE:
                {
                    data  = (void *) OCI_ObjectGetFile(obj, attr);
                    break;
                }
                case OCI_CDT_REF:
                {
                    data  = (void *) OCI_ObjectGetRef(obj, attr);
                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    data  = (void *) OCI_ObjectGetObject(obj, attr);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    data =  (void *) OCI_ObjectGetColl(obj, attr);
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
               
                len += OCI_StringGetFromType(obj->con, &obj->typinf->cols[i], data, data_size, tmpbuf, quote); 
            }
            else
            {
                break;
            }
        }

        if (i < (obj->typinf->nb_cols-1))
        {
            len += OCI_StringAddToBuffer(str, len, OTEXT(", "), quote);
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

