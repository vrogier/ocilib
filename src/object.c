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
    OCI_CALL_ENTER(boolean, FALSE)                                          \
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)                                 \
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)                                \
                                                                            \
    OCI_STATUS = FALSE;                                                     \
                                                                            \
    if (!value)                                                             \
    {                                                                       \
        OCI_STATUS = OCI_ObjectSetNull(obj, attr);                          \
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
            OCI_STATUS = TRUE;                                              \
                                                                            \
            OCI_EXEC(func)                                                  \
                                                                            \
            if (OCI_STATUS)                                                 \
            {                                                               \
                *ind = OCI_IND_NOTNULL;                                     \
            }                                                               \
        }                                                                   \
    }                                                                       \
                                                                            \
    OCI_RETVAL = OCI_STATUS;                                                \
    OCI_CALL_EXIT()

#define OCI_OBJECT_GET_VALUE(datatype, object_type, type, func)             \
                                                                            \
    int index = 0;                                                          \
                                                                            \
    OCI_CALL_ENTER(object_type, NULL)                                       \
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)                                 \
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)                                \
                                                                            \
    OCI_STATUS = FALSE;                                                     \
                                                                            \
    index = OCI_ObjectGetAttrIndex(obj, attr, datatype, TRUE);              \
    if (index >= 0)                                                         \
    {                                                                       \
        OCIInd *ind   = NULL;                                               \
        type   *value = NULL;                                               \
                                                                            \
        OCI_STATUS = TRUE;                                                  \
                                                                            \
        value = (type *) OCI_ObjectGetAttr(obj, index, &ind);               \
                                                                            \
        if (value && ind && (OCI_IND_NULL != *ind))                         \
        {                                                                   \
            OCI_RETVAL = obj->objs[index] = func;                           \
            OCI_STATUS = (NULL != OCI_RETVAL);                              \
        }                                                                   \
    }                                                                       \
                                                                            \
    OCI_CALL_EXIT()

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
    OCI_Object     *obj,
    void           *handle,
    OCI_TypeInfo   *typinf,
    OCI_Object     *parent,
    int             index,
    boolean         reset
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_ALLOCATE_DATA(OCI_IPC_OBJECT, obj, 1);

    if (OCI_STATUS)
    {
        obj->con    = con;
        obj->handle = handle;
        obj->typinf = typinf;

        OCI_ALLOCATE_DATA(OCI_IPC_BUFF_ARRAY, obj->tmpbufs, typinf->nb_cols)
        OCI_ALLOCATE_DATA(OCI_IPC_BUFF_ARRAY, obj->tmpsizes, typinf->nb_cols)
        OCI_ALLOCATE_DATA(OCI_IPC_BUFF_ARRAY, obj->objs, typinf->nb_cols)

        OCI_ObjectReset(obj);

        if (OCI_STATUS && (!obj->handle || (OCI_OBJECT_ALLOCATED_ARRAY == obj->hstate)))
        {
            /* allocates handle for non fetched object */

            if (OCI_OBJECT_ALLOCATED_ARRAY != obj->hstate)
            {
                obj->hstate = OCI_OBJECT_ALLOCATED;
            }

            OCI_EXEC
            (
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

        if (OCI_STATUS && (OCI_UNKNOWN == obj->type))
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

        if (OCI_STATUS && (reset || !obj->tab_ind))
        {
            if (!parent)
            {
                OCI_EXEC
                (
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

    if (!OCI_STATUS && obj)
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

        OCI_FREE(obj->tmpbufs[i])

        obj->tmpsizes[i] = 0;
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
 * OCI_ObjectSetNumberInternal
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ObjectSetNumberInternal
(
    OCI_Object  *obj,
    const otext *attr,
    void        *value,
    uword        size,
    uword        flag
)
{
    int index   = 0;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    OCI_STATUS = FALSE;

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_NUMERIC, TRUE);

    if (index >= 0)
    {
        OCIInd     *ind = NULL;
        void       *num = OCI_ObjectGetAttr(obj, index, &ind);
        OCI_Column *col = &obj->typinf->cols[index];

        OCI_STATUS = TRUE;

        if (OCI_NUM_NUMBER != col->subtype)
        {

            /* for PL/SQL PLS_INTEGER and BINARY_INTEGER types, the values are not OCINumber but of scalar C int type !! */
            
            OCINumber tmp;
            memset(&tmp, 0, sizeof(tmp));

            OCI_STATUS = OCI_STATUS && OCI_NumberSetNativeValue(obj->con, &tmp, col->size, col->subtype, col->libcode, value);
            OCI_STATUS = OCI_STATUS && OCI_NumberGetNativeValue(obj->con, &tmp, size, flag, col->libcode, num);
        }
        else
        {
            OCI_STATUS = OCI_NumberSetNativeValue(obj->con, num, size, flag, col->libcode, value);
        }

        if (OCI_STATUS)
        {
            *ind = OCI_IND_NOTNULL;
        }
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetNumberInternal
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ObjectGetNumberInternal
(
    OCI_Object  *obj,
    const otext *attr,
    void        *value,
    uword        size,
    uword        flag
)
{
    int index   = 0;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)
    
    OCI_STATUS = FALSE;

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_NUMERIC, FALSE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        void   *num = NULL;

        num = OCI_ObjectGetAttr(obj, index, &ind);

        if (num && (OCI_IND_NULL != *ind))
        {
            OCI_Column *col = &obj->typinf->cols[index];
            
            OCI_STATUS = TRUE;

            if (OCI_NUM_NUMBER != col->subtype)
            {
                
                /* for PL/SQL PLS_INTEGER and BINARY_INTEGER types, the values are not OCINumber but of scalar C int type !! */
                
                OCINumber tmp;
                memset(&tmp, 0, sizeof(tmp));

                OCI_STATUS = OCI_STATUS && OCI_NumberSetNativeValue(obj->con, &tmp, col->size, col->subtype, col->libcode, num);
                OCI_STATUS = OCI_STATUS && OCI_NumberGetNativeValue(obj->con, &tmp, size, flag, col->libcode, value);
            }
            else
            {
                OCI_STATUS = OCI_NumberGetNativeValue(obj->con, num, size, flag, col->libcode, value);
            }
        }
    }
    else
    {
        index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TEXT, FALSE);

        if (index >= 0)
        {
            OCI_STATUS = OCI_NumberFromString(obj->con, value, size, flag, obj->typinf->cols[index].libcode,
                                               OCI_ObjectGetString(obj, attr), NULL);
        }
    }

    if (index == -1)
    {
        OCI_ExceptionAttributeNotFound(obj->con, attr);
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(OCI_Object *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_RETVAL = OCI_ObjectInit(con, NULL, NULL, typinf, NULL, -1, TRUE);
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectFree
(
    OCI_Object *obj
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_OBJECT_FETCHED(obj)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    /* if the object has sub-objects that have been fetched, we need to free
       these objects */

    OCI_ObjectReset(obj);

    OCI_FREE(obj->objs)
    OCI_FREE(obj->tmpbufs)
    OCI_FREE(obj->tmpsizes)

    if ((OCI_OBJECT_ALLOCATED == obj->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == obj->hstate))
    {
        OCI_OCIObjectFree(obj->con->env, obj->con->err, obj->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != obj->hstate)
    {
        OCI_FREE(obj)
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(OCI_Object **, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_OBJECT, 0, sizeof(void *), sizeof(OCI_Object), 0, typinf);
    OCI_STATUS = (NULL != arr);

    if (OCI_STATUS)
    {
        OCI_RETVAL = (OCI_Object **) arr->tab_obj;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectArrayFree
(
    OCI_Object **objs
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ARRAY, objs)

    OCI_RETVAL = OCI_STATUS = OCI_ArrayFreeFromHandles((void **)objs);

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj_src);
    OCI_CALL_CHECK_COMPAT(obj->con, obj->typinf->tdo == obj_src->typinf->tdo)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    OCI_EXEC
    (
        OCIObjectCopy(obj->con->env, obj->con->err, obj->con->cxt,
                      obj_src->handle, (obj_src->tab_ind + obj_src->idx_ind),
                      obj->handle, (obj->tab_ind + obj->idx_ind),
                      obj->typinf->tdo, OCI_DURATION_SESSION, OCI_DEFAULT)
    )

    if (OCI_STATUS)
    {
        obj->typinf = obj_src->typinf;

        OCI_ObjectReset(obj);
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    OCI_STATUS = FALSE;

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_BOOLEAN, TRUE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        boolean *value = NULL;

        OCI_STATUS = TRUE;

        value = (boolean *)OCI_ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            OCI_RETVAL = *value;
        }
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetNumber
 * --------------------------------------------------------------------------------------------- */

OCI_Number * OCI_API OCI_ObjectGetNumber
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OCI_OBJECT_GET_VALUE
    (
        OCI_CDT_NUMERIC,
        OCI_Number*,
        OCINumber*,
        OCI_NumberInit(obj->con, (OCI_Number *) obj->objs[index], (OCINumber *) value)
    )
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

    OCI_ObjectGetNumberInternal(obj, attr, &value, sizeof(value), OCI_NUM_SHORT);

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

    OCI_ObjectGetNumberInternal(obj, attr, &value, sizeof(value), OCI_NUM_USHORT);

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

    OCI_ObjectGetNumberInternal(obj, attr, &value, sizeof(value), OCI_NUM_INT);

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

    OCI_ObjectGetNumberInternal(obj, attr, &value, sizeof(value), OCI_NUM_UINT);

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

    OCI_ObjectGetNumberInternal(obj, attr, &value, sizeof(value), OCI_NUM_BIGINT);

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

    OCI_ObjectGetNumberInternal(obj, attr, &value, sizeof(value), OCI_NUM_BIGUINT);

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

    OCI_ObjectGetNumberInternal(obj, attr, &value, sizeof(value), OCI_NUM_DOUBLE);

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

    OCI_ObjectGetNumberInternal(obj, attr, &value, sizeof(value), OCI_NUM_FLOAT);

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

    OCI_CALL_ENTER(const otext *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    OCI_STATUS = FALSE;

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TEXT, FALSE);

    if (index >= 0)
    {
        OCIInd *ind       = NULL;
        OCIString **value = NULL;

        OCI_STATUS = TRUE;

        value = (OCIString **) OCI_ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            if (OCILib.use_wide_char_conv)
            {
                OCI_RETVAL = OCI_StringFromStringPtr(obj->con->env, *value, &obj->tmpbufs[index], &obj->tmpsizes[index]);
            }
            else
            {
                OCI_RETVAL = (otext *)OCIStringPtr(obj->con->env, *value);
            }
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

            OCI_STATUS = TRUE;

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
                OCI_STATUS = OCI_StringRequestBuffer(&obj->tmpbufs[index], &obj->tmpsizes[index], len);

                if (OCI_STATUS)
                {
                    OCI_StringGetFromType(obj->con, &obj->typinf->cols[index], value, size, obj->tmpbufs[index], FALSE);
                    OCI_RETVAL = obj->tmpbufs[index];
                }
            }
        }
    }

    if (index == -1)
    {
        OCI_ExceptionAttributeNotFound(obj->con, attr);
    }

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(int, 0);
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    OCI_STATUS = FALSE;

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_RAW, TRUE);

    if (index >= 0)
    {
        OCIInd *ind    = NULL;
        OCIRaw **value = NULL;

        OCI_STATUS = TRUE;

        value = (OCIRaw **) OCI_ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            ub4 raw_len = OCIRawSize(obj->con->env, *value);

            if (len > raw_len)
            {
                len = raw_len;
            }

            memcpy(buffer, OCIRawPtr(obj->con->env, *value), (size_t) len);

            OCI_RETVAL = len;
        }
    }

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    OCI_STATUS = FALSE;

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_RAW, TRUE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        OCIRaw **value = NULL;

        OCI_STATUS = TRUE;

        value = (OCIRaw **)OCI_ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            raw_len = OCIRawSize(obj->con->env, *value);
        }
    }

    OCI_RETVAL = raw_len;

    OCI_CALL_EXIT()
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
        OCI_DateInit(obj->con, (OCI_Date *) obj->objs[index], value, FALSE, FALSE)
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
        OCI_TimestampInit(obj->con, (OCI_Timestamp *) obj->objs[index],
                          (OCIDateTime *) *value, obj->typinf->cols[index].subtype)
    )

#else

    OCI_CALL_ENTER( OCI_Timestamp *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_EXIT()

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
        OCI_IntervalInit(obj->con, (OCI_Interval *) obj->objs[index],
                         (OCIInterval *) *value, obj->typinf->cols[index].subtype)
    )

#else

    OCI_CALL_ENTER(OCI_Interval *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_EXIT()

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
        OCI_CollInit(obj->con, (OCI_Coll *) obj->objs[index], (OCIColl *) *value, obj->typinf->cols[index].typinf)
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
        OCI_ObjectInit(obj->con, (OCI_Object *) obj->objs[index], value,
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
        OCI_LobInit(obj->con, (OCI_Lob *) obj->objs[index], *value, obj->typinf->cols[index].subtype)
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
        OCI_FileInit(obj->con, (OCI_File *) obj->objs[index], *value, obj->typinf->cols[index].subtype)
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
        OCI_RefInit(obj->con, NULL, (OCI_Ref *) obj->objs[index], *value)
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    OCI_STATUS = FALSE;

    index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_BOOLEAN, TRUE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        boolean *data = (boolean *)OCI_ObjectGetAttr(obj, index, &ind);

        if (data)
        {
            *data = value;
            *ind = OCI_IND_NOTNULL;

            OCI_STATUS = TRUE;
        }
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectSetNumber
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_ObjectSetNumber
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Number  *value
)
{
    OCI_OBJECT_SET_VALUE
    (
        OCI_CDT_NUMERIC,
        OCINumber,
        OCINumberAssign( obj->con->err, value->handle, data)
    )
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
    return OCI_ObjectSetNumberInternal(obj, attr, &value, sizeof(value), (uword) OCI_NUM_SHORT);
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
    return OCI_ObjectSetNumberInternal(obj, attr, &value, sizeof(value), (uword) OCI_NUM_USHORT);
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
    return OCI_ObjectSetNumberInternal(obj, attr, &value, sizeof(value), (uword) OCI_NUM_INT);
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
    return OCI_ObjectSetNumberInternal(obj, attr, &value, sizeof(value), (uword) OCI_NUM_UINT);
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
    return OCI_ObjectSetNumberInternal(obj, attr, &value, sizeof(value), (uword) OCI_NUM_BIGINT);
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
    return OCI_ObjectSetNumberInternal(obj, attr, &value, sizeof(value), (uword) OCI_NUM_BIGUINT);
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
    return OCI_ObjectSetNumberInternal(obj, attr, &value, sizeof(value), (uword) OCI_NUM_DOUBLE);
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
    return OCI_ObjectSetNumberInternal(obj, attr, &value, sizeof(value), (uword) OCI_NUM_FLOAT);
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    OCI_STATUS = FALSE;

    if (!value)
    {
        OCI_STATUS = OCI_ObjectSetNull(obj, attr);
    }
    else
    {
        int index = OCI_ObjectGetAttrIndex(obj, attr, OCI_CDT_TEXT, TRUE);

        if (index >= 0)
        {
            OCIInd *ind      = NULL;
            OCIString **data = (OCIString **) OCI_ObjectGetAttr(obj, index, &ind);

            OCI_STATUS = OCI_StringToStringPtr(obj->con->env, data, obj->con->err, value);

            if (OCI_STATUS)
            {
                *ind = OCI_IND_NOTNULL;
            }
        }
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_EXIT()

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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_EXIT()

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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    index = OCI_ObjectGetAttrIndex(obj, attr, -1, TRUE);

    if (index >= 0)
    {
        int ind_index = obj->idx_ind + OCI_ObjectGetIndOffset(obj->typinf, index);

        obj->tab_ind[ind_index] = OCI_IND_NULL;

        OCI_STATUS = TRUE;
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    index = OCI_ObjectGetAttrIndex(obj, attr, -1, TRUE);

    if (index >= 0)
    {
        int ind_index = obj->idx_ind + OCI_ObjectGetIndOffset(obj->typinf, index);

        OCI_RETVAL = (OCI_IND_NOTNULL != obj->tab_ind[ind_index]);

        OCI_STATUS = TRUE;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OCI_API OCI_ObjectGetTypeInfo
(
    OCI_Object *obj
)
{
    OCI_GET_PROP(OCI_TypeInfo*, NULL, OCI_IPC_OBJECT, obj, typinf, obj->con, NULL, obj->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ObjectGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ObjectGetType
(
    OCI_Object *obj
)
{
    OCI_GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_OBJECT, obj, type, obj->con, NULL, obj->con->err)
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CHECK_COMPAT(obj->con, obj->typinf->tdo == ref->typinf->tdo)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    OCI_EXEC(OCIObjectGetObjectRef(obj->con->env, obj->con->err, obj->handle, ref->handle))

    if (OCI_STATUS && ref->obj)
    {
        OCI_ObjectFree(ref->obj);
        ref->obj = NULL;
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

    if (pp_struct)
    {
        *pp_struct = (void *) obj->handle;
    }

    if (pp_ind)
    {
        *pp_ind = (void *) obj->tab_ind;
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, size)
    OCI_CALL_CONTEXT_SET_FROM_CONN(obj->con)

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

            OCI_STATUS = (NULL != data);

            if (OCI_STATUS)
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

    if (OCI_STATUS)
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

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

