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

#define OCI_OBJECT_SET_VALUE(datatype, type, func)                          \
                                                                            \
    OCI_LIB_CALL_ENTER(boolean, FALSE)                                      \
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)                                      \
                                                                            \
    if (!value)                                                             \
    {                                                                       \
        call_status = OCI_ObjectSetNull(obj, attr);                         \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        int index = OCI_ObjectGetAttrIndex(obj, attr, datatype, TRUE);      \
                                                                            \
        if (index >= 0)                                                     \
        {                                                                   \
            OCIInd  *ind  = NULL;                                           \
            type    *data = (type *)  OCI_ObjectGetAttr(obj, index, &ind);  \
                                                                            \
            call_status = TRUE;                                             \
                                                                            \
            OCI_CALL2(call_status, obj->con, func)                          \
                                                                            \
            if (call_status)                                                \
            {                                                               \
                *ind = OCI_IND_NOTNULL;                                     \
            }                                                               \
        }                                                                   \
    }                                                                       \
    call_retval = call_status;                                              \
    OCI_LIB_CALL_EXIT()

#define OCI_OBJECT_GET_VALUE(datatype, object_type, type, func)             \
                                                                            \
    int index = 0;                                                          \
    OCI_LIB_CALL_ENTER(object_type, NULL)                                   \
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)                                      \
                                                                            \
    index = OCI_ObjectGetAttrIndex(obj, attr, datatype, TRUE);              \
    if (index >= 0)                                                         \
    {                                                                       \
        OCIInd *ind   = NULL;                                               \
        type   *value = NULL;                                               \
                                                                            \
        call_status = TRUE;                                                 \
                                                                            \
        value = (type *) OCI_ObjectGetAttr(obj, index, &ind);               \
                                                                            \
        if (value && ind && (OCI_IND_NULL != *ind))                         \
        {                                                                   \
            call_retval = func;                                             \
            call_status = (NULL != call_retval);                            \
        }                                                                   \
    }                                                                       \
    OCI_LIB_CALL_EXIT()

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

    OCI_CHECK(typinf == NULL, 0);

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

void OCI_ObjectGetStructSize
(
    OCI_TypeInfo *typinf,
    size_t       *p_size,
    size_t       *p_align
)
{

    if (!typinf || !p_size || !p_align)
    {
        return;
    }

    if (typinf->struct_size == 0)
    {
        size_t size  = 0;
        size_t size1 = 0;
        size_t size2 = 0;
        size_t align = 0;

        ub2 i;

        for (i = 0; i < typinf->nb_cols; i++)
        {
            if (i > 0)
            {
                size1 = size2;

                typinf->offsets[i] = (int) size;
            }
            else
            {
                OCI_ObjectGetAttrInfo(typinf, i, &size1, &align);

                typinf->offsets[i] = 0;
            }

            OCI_ObjectGetAttrInfo(typinf, i + 1, &size2, &align);

            if (align > typinf->align)
            {
                typinf->align = align;
            }

            size += size1;

            size = ROUNDUP(size, align);
        }

        typinf->struct_size = ROUNDUP(size + size2, typinf->align);
    }

    *p_size  = typinf->struct_size;
    *p_align = typinf->align;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetUserStructSize
 * --------------------------------------------------------------------------------------------- */

void OCI_ObjectGetUserStructSize
(
    OCI_TypeInfo *typinf,
    size_t       *p_size,
    size_t       *p_align
)
{
    size_t size1  = 0;
    size_t size2  = 0;
    size_t align1 = 0;
    size_t align2 = 0;
    size_t align  = 0;

    ub2 i;

    size_t size = 0;

    if (!typinf || !p_size || !p_align)
    {
        return;
    }

    for (i = 0; i < typinf->nb_cols; i++)
    {
        OCI_ColumnGetAttrInfo(&typinf->cols[i],   typinf->nb_cols, i, &size1, &align1);
        OCI_ColumnGetAttrInfo(&typinf->cols[i+1], typinf->nb_cols, i+1, &size2, &align2);

        if (align < align1)
        {
            align = align1;
        }

        if (align < align2)
        {
            align = align2;
        }

        size += size1;

        size = ROUNDUP(size, align2);
    }

    *p_size  = size;
    *p_align = align;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetAttrInfo
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ObjectGetAttrInfo
(
    OCI_TypeInfo *typinf,
    int           index,
    size_t       *p_size,
    size_t       *p_align
)
{
    OCI_CHECK(typinf  == NULL, 0);
    OCI_CHECK(p_size  == NULL, 0);
    OCI_CHECK(p_align == NULL, 0);


    if (index >= typinf->nb_cols)
    {
        *p_size = 0;

        return FALSE;
    }

    switch (typinf->cols[index].datatype)
    {
        case OCI_CDT_NUMERIC:
        {
            ub4 subtype = typinf->cols[index].subtype;

            if (subtype & OCI_NUM_SHORT)
            {
                *p_size  = sizeof(short);
                *p_align = *p_size;
            }
            else if (subtype & OCI_NUM_INT)
            {
                *p_size  = sizeof(int);
                *p_align = *p_size;
            }
            else if (subtype & OCI_NUM_FLOAT)
            {
                *p_size  = sizeof(double);
                *p_align = *p_size;
            }
            else if (subtype & OCI_NUM_DOUBLE)
            {
                *p_size  = sizeof(double);
                *p_align = *p_size;
            }
            else
            {
                *p_size  = sizeof(OCINumber);
                *p_align = sizeof(ub1);
            }
            break;
        }
        case OCI_CDT_DATETIME:
        {
            *p_size  = sizeof(OCIDate);
            *p_align = sizeof(sb2);
            break;
        }
        case OCI_CDT_BOOLEAN:
        {
            *p_size  = sizeof(boolean);
            *p_align = *p_size;
            break;
        }
        case OCI_CDT_OBJECT:
        {
            OCI_ObjectGetStructSize(typinf->cols[index].typinf, p_size, p_align);
            break;
        }
        default:
        {
            *p_size  = sizeof(void *);
            *p_align = *p_size;
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
    boolean      res = FALSE;

    OCI_CHECK(NULL == pobj, NULL)

    if (!*pobj)
    {
        *pobj = (OCI_Object *) OCI_MemAlloc(OCI_IPC_OBJECT, sizeof(*obj), (size_t) 1, TRUE);
    }

    if (*pobj)
    {
        res = TRUE;

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

        res = (NULL != obj->objs);

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
                              (OCITypeCode) obj->typinf->typecode, obj->typinf->tdo, (dvoid *) NULL,
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
               parent objects leads to a segmentation fault on MS Windows !
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

    /* check for failure */

    if (!res && obj)
    {
        OCI_ObjectFree(obj);
        *pobj = obj = NULL;
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

    if (!obj)
    {
        return;
    }
   
    for (i = 0; i < obj->typinf->nb_cols; i++)
    {
        if (obj->objs[i])
        {
            OCI_Datatype * data = (OCI_Datatype *) obj->objs[i];

            if (OCI_OBJECT_FETCHED_CLEAN == data->hstate)
            {
                data->hstate =  OCI_OBJECT_FETCHED_DIRTY;
            }

            OCI_FreeObjectFromType(obj->objs[i], obj->typinf->cols[i].datatype);
            
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
    int          type,
    boolean      check
)
{
    int res = -1;
    ub2 i;

    OCI_CHECK(obj  == NULL, res)
    OCI_CHECK(attr == NULL, res);

    for (i = 0; i < obj->typinf->nb_cols; i++)
    {
        OCI_Column *col = &obj->typinf->cols[i];

        if (((type == -1) || (col->datatype == type))  && (ostrcasecmp(col->name, attr) == 0))
        {
            res = (int) i;
            break;
        }
    }

    if (check && res == -1)
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
    size_t size   = 0;
    size_t align  = 0;

    OCI_CHECK(obj  == NULL, NULL)
    OCI_CHECK(pind == NULL, NULL)

    if (obj->typinf->struct_size == 0)
    {
        OCI_ObjectGetStructSize(obj->typinf, &size, &align);
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
    int index   = 0;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_STRING, attr)

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_NUMERIC, TRUE);

    if (index >= 0)
    {
        OCIInd     *ind = NULL;
        void       *num = OCI_ObjectGetAttr(obj, index, &ind);
        OCI_Column *col = &obj->typinf->cols[index];

        call_status = TRUE;

        if (OCI_NUM_NUMBER != col->subtype)
        {

            /* for PL/SQL PLS_INTEGER and BINARY_INTEGER types, the values are not OCINumber but of scalar C int type !! */
            
            OCINumber tmp;
            memset(&tmp, 0, sizeof(tmp));

            call_status = call_status && OCI_NumberSet(obj->con, &tmp, col->size, col->subtype, col->libcode, value);
            call_status = call_status && OCI_NumberGet(obj->con, &tmp, size, flag, col->libcode, num);
        }
        else
        {
            call_status = OCI_NumberSet(obj->con, num, size, flag, col->libcode, value);
        }

        if (call_status)
        {
            *ind = OCI_IND_NOTNULL;
        }
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    int index   = 0;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_STRING, attr)

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_NUMERIC, FALSE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        void   *num = NULL;

        num = OCI_ObjectGetAttr(obj, index, &ind);

        if (num && (OCI_IND_NULL != *ind))
        {
            OCI_Column *col = &obj->typinf->cols[index];
            
            call_status = TRUE;

            if (OCI_NUM_NUMBER != col->subtype)
            {
                
                /* for PL/SQL PLS_INTEGER and BINARY_INTEGER types, the values are not OCINumber but of scalar C int type !! */
                
                OCINumber tmp;
                memset(&tmp, 0, sizeof(tmp));

                call_status = call_status && OCI_NumberSet(obj->con, &tmp, col->size, col->subtype, col->libcode, num);
                call_status = call_status && OCI_NumberGet(obj->con, &tmp, size, flag, col->libcode, value);
            }
            else
            {
                call_status = OCI_NumberGet(obj->con, num, size, flag, col->libcode, value);
            }
        }
    }
    else
    {
        index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TEXT, FALSE);

        if (index >= 0)
        {
            call_status = OCI_NumberFromString(obj->con, value, size, flag, obj->typinf->cols[index].libcode,
                                               OCI_ObjectGetString(obj, attr), NULL);
        }
    }

    if (index == -1)
    {
        OCI_ExceptionAttributeNotFound(obj->con, attr);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(OCI_Object *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)

    call_retval = OCI_ObjectInit(con, &call_retval, NULL, typinf, NULL, -1, TRUE);
    call_status = (NULL != call_retval);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectFree
(
    OCI_Object *obj
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_OBJECT_FETCHED(obj)

    /* if the object has sub-objects that have been fetched, we need to free
       these objects */

    OCI_ObjectReset(obj);

    if (obj->objs)
    {
        OCI_FREE(obj->objs)
    }

    if ((OCI_OBJECT_ALLOCATED == obj->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == obj->hstate))
    {
        OCI_OCIObjectFree(obj->con->env, obj->con->err, obj->handle, OCI_DEFAULT);
    }

    OCI_FREE(obj->tmpbuf)

    if (OCI_OBJECT_ALLOCATED_ARRAY != obj->hstate)
    {
        OCI_FREE(obj)
    }

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    OCI_Array *arr = NULL;

    OCI_LIB_CALL_ENTER(OCI_Object **, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_PTR(OCI_IPC_TYPE_INFO, con)

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_OBJECT, 0, sizeof(void *), sizeof(OCI_Object), 0, typinf);

    if (arr)
    {
        call_retval = (OCI_Object **) arr->tab_obj;
        call_status = TRUE;
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectArrayFree
(
    OCI_Object **objs
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ARRAY, objs)

    call_retval = call_status = OCI_ArrayFreeFromHandles((void **)objs);

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj_src);

    OCI_CHECK_COMPAT(obj->con, obj->typinf->tdo == obj_src->typinf->tdo)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, obj->con,

        OCIObjectCopy(obj->con->env, obj->con->err, obj->con->cxt,
                      obj_src->handle, (obj_src->tab_ind + obj_src->idx_ind),
                      obj->handle, (obj->tab_ind + obj->idx_ind),
                      obj->typinf->tdo, OCI_DURATION_SESSION, OCI_DEFAULT)
    )

    if (call_status)
    {
        obj->typinf = obj_src->typinf;

        OCI_ObjectReset(obj);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetShort
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectGetBoolean
(
    OCI_Object  *obj,
    const otext *attr
)
{
    int index = -1;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_STRING, attr)

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_BOOLEAN, TRUE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        boolean *value = NULL;

        call_status = TRUE;

        value = (boolean *)OCI_ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            call_retval = *value;
        }
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetShort
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
 * OCI_ObjectGetUnsignedShort
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
    int index = -1;

    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_STRING, attr)

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TEXT, FALSE);

    if (index >= 0)
    {
        OCIInd *ind       = NULL;
        OCIString **value = NULL;

        call_status = TRUE;

        value = (OCIString **) OCI_ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            call_retval = OCI_StringFromStringPtr(obj->con->env, *value,  &obj->tmpbuf, &obj->tmpsize);
        }
    }
    else
    {
        index = OCI_ObjectGetAttrIndex(obj, attr, -1, FALSE);

        if (index >= 0)
        {
            OCIInd      *ind   = NULL;
            void        *value = NULL;
            unsigned int size  = 0;
            unsigned int len   = 0;

            call_status = TRUE;

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
                call_status = OCI_StringRequestBuffer(&obj->tmpbuf, &obj->tmpsize, len);

                if (call_status)
                {
                    OCI_StringGetFromType(obj->con, &obj->typinf->cols[index], value, size, obj->tmpbuf, FALSE);
                    call_retval = obj->tmpbuf;
                }
            }
        }
    }

    if (index == -1)
    {
        OCI_ExceptionAttributeNotFound(obj->con, attr);
    }

    OCI_LIB_CALL_EXIT()
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
    int index = -1;

    OCI_LIB_CALL_ENTER(int, 0);

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_STRING, attr)

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_RAW, TRUE);

    if (index >= 0)
    {
        OCIInd *ind    = NULL;
        OCIRaw **value = NULL;

        call_status = TRUE;

        value = (OCIRaw **) OCI_ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            ub4 raw_len = OCIRawSize(obj->con->env, *value);

            if (len > raw_len)
            {
                len = raw_len;
            }

            memcpy(buffer, OCIRawPtr(obj->con->env, *value), (size_t) len);

            call_retval = len;
        }
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ObjectGetRawSize
* --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ObjectGetRawSize
(
    OCI_Object  *obj,
    const otext *attr
)
{
    ub4 raw_len = 0;
    int index = -1;

    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_STRING, attr)

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_RAW, TRUE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        OCIRaw **value = NULL;

        call_status = TRUE;

        value = (OCIRaw **)OCI_ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            raw_len = OCIRawSize(obj->con->env, *value);
        }
    }

    call_retval = raw_len;

    OCI_LIB_CALL_EXIT()
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
    OCI_OBJECT_GET_VALUE
    (
        OCI_CDT_DATETIME,
        OCI_Date*,
        OCIDate,
        OCI_DateInit(obj->con, (OCI_Date **) &obj->objs[index], value, FALSE, FALSE)
    )
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
#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_OBJECT_GET_VALUE
    (
        OCI_CDT_TIMESTAMP,
        OCI_Timestamp*,
        OCIDateTime*,
        OCI_TimestampInit(obj->con, (OCI_Timestamp **) &obj->objs[index],
                          (OCIDateTime *) *value, obj->typinf->cols[index].subtype)
    )

#else

    OCI_LIB_CALL_ENTER( OCI_Timestamp *, NULL)
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    call_status = TRUE;
    OCI_LIB_CALL_EXIT()

#endif
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
#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_OBJECT_GET_VALUE
        (
        OCI_CDT_INTERVAL,
        OCI_Interval*,
        OCIInterval *,
        OCI_IntervalInit(obj->con, (OCI_Interval **) &obj->objs[index],
                         (OCIInterval *) *value, obj->typinf->cols[index].subtype)
    )

#else

    OCI_LIB_CALL_ENTER(OCI_Interval *, NULL)
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    call_status = TRUE;
    OCI_LIB_CALL_EXIT()

#endif
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
    OCI_OBJECT_GET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCI_Coll*,
        OCIColl*,
        OCI_CollInit(obj->con, (OCI_Coll **) &obj->objs[index],
                     (OCIColl *) *value, obj->typinf->cols[index].typinf)
    )
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
    OCI_OBJECT_GET_VALUE
    (
        OCI_CDT_OBJECT,
        OCI_Object*,
        void,
        OCI_ObjectInit(obj->con, (OCI_Object **) &obj->objs[index], value,
                       obj->typinf->cols[index].typinf,  obj, index, FALSE)
    )
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
    OCI_OBJECT_GET_VALUE
    (
        OCI_CDT_LOB,
        OCI_Lob*,
        OCILobLocator*,
        OCI_LobInit(obj->con, (OCI_Lob **) &obj->objs[index], *value, obj->typinf->cols[index].subtype)
    )
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
    OCI_OBJECT_GET_VALUE
    (
        OCI_CDT_FILE,
        OCI_File *,
        OCILobLocator*,
        OCI_FileInit(obj->con, (OCI_File **) &obj->objs[index], *value, obj->typinf->cols[index].subtype)
    )
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
    OCI_OBJECT_GET_VALUE
    (
        OCI_CDT_REF,
        OCI_Ref*,
        OCIRef*,
        OCI_RefInit(obj->con, NULL, (OCI_Ref **) &obj->objs[index], *value)
    )
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ObjectSetBoolean
* --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetBoolean
(
    OCI_Object  *obj,
    const otext *attr,
    boolean      value
)
{
    int index;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_STRING, attr)

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_BOOLEAN, TRUE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        boolean *data = (boolean *)OCI_ObjectGetAttr(obj, index, &ind);

        if (data)
        {
            *data = value;
            *ind = OCI_IND_NOTNULL;

            call_status = TRUE;
        }
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_STRING, attr)

    if (!value)
    {
        call_status = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TEXT, TRUE);

        if (index >= 0)
        {
            OCIInd *ind      = NULL;
            OCIString **data = (OCIString **) OCI_ObjectGetAttr(obj, index, &ind);

            call_status = OCI_StringToStringPtr(obj->con->env, data, obj->con->err, value);

            if (call_status)
            {
                *ind = OCI_IND_NOTNULL;
            }
        }
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_RAW,
        OCIRaw*,
        OCIRawAssignBytes(obj->con->env, obj->con->err, (ub1*) value, len, data)
    )
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
    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_DATETIME,
        OCIDate,
        OCIDateAssign(obj->con->err, value->handle, data)
    )
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
#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_TIMESTAMP,
        OCIDateTime*,
        OCIDateTimeAssign((dvoid *) obj->con->env, obj->con->err, value->handle, *data)
    )

#else

    OCI_LIB_CALL_ENTER(boolean, FALSE)
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    call_status = TRUE;
    OCI_LIB_CALL_EXIT()

#endif
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
#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_INTERVAL,
        OCIInterval*,
        OCIIntervalAssign((dvoid *) obj->con->env, obj->con->err, value->handle, *data)
    )

#else

    OCI_LIB_CALL_ENTER(boolean, FALSE)
    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    call_status = TRUE;
    OCI_LIB_CALL_EXIT()

#endif
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
    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCIColl*,
        OCICollAssign(obj->con->env, obj->con->err, value->handle, *data)
    )
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
    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_OBJECT,
        void,
        OCIObjectCopy(obj->con->env, obj->con->err, obj->con->cxt,
                     value->handle, (value->tab_ind + value->idx_ind),
                     data, ind, obj->typinf->cols[index].typinf->tdo,
                     OCI_DURATION_SESSION, OCI_DEFAULT)
    )
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
    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_LOB,
        OCILobLocator*,
        OCILobLocatorAssign(obj->con->cxt, obj->con->err, value->handle, (OCILobLocator **) data)
    )
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
    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_FILE,
        OCILobLocator*,
        OCILobLocatorAssign(obj->con->cxt, obj->con->err, value->handle, (OCILobLocator **) data)
    )
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
    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_REF,
        OCIRef*,
        OCIRefAssign(obj->con->env, obj->con->err, value->handle, data)
    )
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
    int index;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_STRING, attr)

    index = OCI_ObjectGetAttrIndex(obj, attr, -1, TRUE);

    if (index >= 0)
    {
        int ind_index = obj->idx_ind + OCI_ObjectGetIndOffset(obj->typinf, index);

        obj->tab_ind[ind_index] = OCI_IND_NULL;

        call_status = TRUE;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    int index   = 0;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_STRING, attr)

    index = OCI_ObjectGetAttrIndex(obj, attr, -1, TRUE);

    if (index >= 0)
    {
        int ind_index = obj->idx_ind + OCI_ObjectGetIndOffset(obj->typinf, index);

        call_retval = (OCI_IND_NOTNULL != obj->tab_ind[ind_index]);

        call_status = TRUE;
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OCI_API OCI_ObjectGetTypeInfo
(
    OCI_Object *obj
)
{
    OCI_LIB_CALL_ENTER(OCI_TypeInfo*, NULL)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)

    call_retval = obj->typinf;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ObjectGetType
(
    OCI_Object *obj
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)

    call_retval = (unsigned int) obj->type;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CHECK_COMPAT(obj->con, obj->typinf->tdo == ref->typinf->tdo)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, obj->con,

        OCIObjectGetObjectRef(obj->con->env, obj->con->err, obj->handle, ref->handle)
    )

    if (call_status && ref->obj)
    {
        OCI_ObjectFree(ref->obj);
        ref->obj = NULL;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)

    if (pp_struct)
    {
        *pp_struct = (void *) obj->handle;
    }

    if (pp_ind)
    {
        *pp_ind = (void *) obj->tab_ind;
    }

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    otext       *attr  = NULL;
    boolean      quote = TRUE;
    unsigned int len   = 0;

    int i;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CHECK_PTR(OCI_IPC_VOID, size)

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
                case OCI_CDT_BOOLEAN:
                {
                    OCIInd *ind = NULL;
                    data = OCI_ObjectGetAttr(obj, i, &ind);
                    quote = FALSE;
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

            call_status = (NULL != data);

            if (call_status)
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

    if (call_status)
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

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

