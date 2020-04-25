/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2020 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "object.h"

#include "array.h"
#include "collection.h"
#include "column.h"
#include "date.h"
#include "file.h"
#include "helpers.h"
#include "interval.h"
#include "list.h"
#include "lob.h"
#include "macros.h"
#include "memory.h"
#include "number.h"
#include "reference.h"
#include "strings.h"
#include "timestamp.h"
#include "typeinfo.h"

#define OBJECT_SET_VALUE(datatype, type, func)                              \
                                                                            \
    CALL_ENTER(boolean, FALSE)                                              \
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)                                     \
    CALL_CONTEXT_FROM_CON(obj->con)                                         \
                                                                            \
    STATUS = FALSE;                                                         \
                                                                            \
    if (!value)                                                             \
    {                                                                       \
        STATUS = ObjectSetNull(obj, attr);                                  \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        int index = ObjectGetAttributeIndex(obj, attr, datatype, TRUE);     \
                                                                            \
        if (index >= 0)                                                     \
        {                                                                   \
            OCIInd  *ind  = NULL;                                           \
            type    *data = (type *)  ObjectGetAttr(obj, index, &ind);      \
                                                                            \
            STATUS = TRUE;                                                  \
                                                                            \
            EXEC(func)                                                      \
                                                                            \
            if (STATUS)                                                     \
            {                                                               \
                *ind = OCI_IND_NOTNULL;                                     \
            }                                                               \
        }                                                                   \
    }                                                                       \
                                                                            \
    RETVAL = STATUS;                                                        \
    CALL_EXIT()

#define OBJECT_GET_VALUE(datatype, object_type, type, func)                 \
                                                                            \
    int index = 0;                                                          \
                                                                            \
    CALL_ENTER(object_type, NULL)                                           \
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)                                     \
    CALL_CONTEXT_FROM_CON(obj->con)                                         \
                                                                            \
    STATUS = FALSE;                                                         \
                                                                            \
    index = ObjectGetAttributeIndex(obj, attr, datatype, TRUE);             \
    if (index >= 0)                                                         \
    {                                                                       \
        OCIInd *ind   = NULL;                                               \
        type   *value = NULL;                                               \
                                                                            \
        STATUS = TRUE;                                                      \
                                                                            \
        value = (type *) ObjectGetAttr(obj, index, &ind);                   \
                                                                            \
        if (value && ind && (OCI_IND_NULL != *ind))                         \
        {                                                                   \
            RETVAL = obj->objs[index] = func;                               \
            STATUS = (NULL != RETVAL);                                      \
        }                                                                   \
    }                                                                       \
                                                                            \
    CALL_EXIT()

// pre declaration

boolean ObjectGetAttributeInfo
(
    OCI_TypeInfo * typinf,
    int           index,
    size_t * p_size,
    size_t * p_align
);

 /* --------------------------------------------------------------------------------------------- *
 * ObjectGetRealTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * ObjectGetRealTypeInfo(OCI_TypeInfo *typinf, void *object)
{
    DECLARE_CTX(TRUE)

    OCI_TypeInfo *result = typinf;

    if (!result)
    {
        return result;
    }

    CALL_CONTEXT_FROM_CON(result->con)

    /* if the type is related to UTDs and is virtual (e.g. non final), we must find the real type of the instance */

    if (object && result->type == OCI_TIF_TYPE && !result->is_final)
    {
        OCIRef  *ref = NULL;
        OCIType *tdo = NULL;

        /* create a local REF to store a REF to the object real type */

        EXEC(MemoryAllocateObject(result->con->env, result->con->err, result->con->cxt, SQLT_REF, (OCIType *)0, NULL, OCI_DURATION_SESSION, 0, (void**)&ref))
        EXEC(OCIObjectGetTypeRef(result->con->env, result->con->err, (dvoid*)object, ref))
        EXEC(OCITypeByRef(result->con->env, result->con->err, ref, OCI_DURATION_SESSION, OCI_TYPEGET_HEADER, &tdo))

        /* the object instance type pointer is different only if the instance is from an inherited type */

        if (tdo && tdo != result->tdo)
        {
            /* first try to find it in list */
            const boolean found = ListExists(typinf->con->tinfs, tdo);

            if (!found)
            {
                OCIDescribe *descr = NULL;
                OCIParam    *param = NULL;
                otext *schema_name = NULL;
                otext *object_name = NULL;

                unsigned int size_schema = 0;
                unsigned int size_object = 0;

                otext fullname[(OCI_SIZE_OBJ_NAME * 2) + 2] = OTEXT("");

                STATUS = MemoryAllocHandle(result->con->env, (void**) &descr, OCI_HTYPE_DESCRIBE);

                EXEC(OCIDescribeAny(result->con->cxt, result->con->err, (dvoid *)tdo, 0, OCI_OTYPE_PTR, OCI_DEFAULT, OCI_PTYPE_UNK, descr))
                ATTRIB_GET(OCI_HTYPE_DESCRIBE, OCI_ATTR_PARAM, descr, &param, NULL)

                STATUS = STATUS && StringGetAttribute(result->con, param, OCI_DTYPE_PARAM, OCI_ATTR_SCHEMA_NAME, &schema_name, &size_schema);
                STATUS = STATUS && StringGetAttribute(result->con, param, OCI_DTYPE_PARAM, OCI_ATTR_NAME, &object_name, &size_object);

                if (STATUS)
                {
                    /* compute link full name */

                    StringGetFullTypeName(schema_name, NULL, object_name, NULL, fullname, (sizeof(fullname) / sizeof(otext)) - 1);

                    /* retrieve the type info of the real object */

                    result = TypeInfoGet(result->con, fullname, OCI_TIF_TYPE);
                }

                MemoryFreeHandle(descr, OCI_HTYPE_DESCRIBE);
            }
        }

        /* free local REF */

        MemoryFreeObject(result->con->env, result->con->err, ref, OCI_DEFAULT);
    }

    return result;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetIndicatorOffset
 * --------------------------------------------------------------------------------------------- */

ub2 ObjectGetIndOffset
(
    OCI_TypeInfo *typinf,
    int           index
)
{
    ub2 i = 0, j = 1;

    CHECK(typinf == NULL, 0);

    for (i = 0; i < (int) index; i++)
    {
        if (OCI_CDT_OBJECT == typinf->cols[i].datatype)
        {
            j += ObjectGetIndOffset(typinf->cols[i].typinf, typinf->cols[i].typinf->nb_cols);
        }
        else
        {
            j++;
        }

    }

    return j;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetStructSize
 * --------------------------------------------------------------------------------------------- */

void ObjectGetStructSize
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

        ub2 i = 0;

        /* if the type is a sub type, then it is a subset containing all of his parent members
           In that case the first member is a buffer holding a parent object structure */

        if (typinf->parent_type)
        {
            /* if super type information has not been already cached, then let's compute it now */
            
            ObjectGetStructSize(typinf->parent_type, &size, &align);

            /* copy super type members offsets to the current sub type of members offsets */

            for (; i < typinf->parent_type->nb_cols; i++)
            {
                typinf->offsets[i] = typinf->parent_type->offsets[i];
            }

            /* adjust current member index to start to compute with the first of the derived type */
            
            i = typinf->parent_type->nb_cols;

            /* compute the first derived member in order to not touch to the next for loop code that is working :) */
            
            if (i < typinf->nb_cols)
            {
                size_t next_align = 0;

                /* set current alignment to the parent one as it is the first member of the current structure */

                typinf->align = align;

                /* get current type self first member information (after parent type) */

                ObjectGetAttributeInfo(typinf, i, &size2, &next_align);

                /* make sure that parent field is aligned */

                size = ROUNDUP(size, next_align);
            }
        }

        for (; i < typinf->nb_cols; i++)
        {
            if (i > 0)
            {
                size1 = size2;

                typinf->offsets[i] = (int) size;
            }
            else
            {
                ObjectGetAttributeInfo(typinf, i, &size1, &align);

                typinf->offsets[i] = 0;
            }

            ObjectGetAttributeInfo(typinf, i + 1, &size2, &align);

            size += size1;

            size = ROUNDUP(size, align);
        }

        typinf->struct_size = ROUNDUP(size + size2, typinf->align);
    }

    *p_size  = typinf->struct_size;
    *p_align = typinf->align;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetUserStructSize
 * --------------------------------------------------------------------------------------------- */

void ObjectGetUserStructSize
(
    OCI_TypeInfo* typinf,
    size_t* p_size,
    size_t* p_align
)
{
    size_t size1  = 0;
    size_t size2  = 0;
    size_t align1 = 0;
    size_t align2 = 0;
    size_t align  = 0;

    size_t size = 0;

    if (!typinf || !p_size || !p_align)
    {
        return;
    }

    for (ub2 i = 0; i < typinf->nb_cols; i++)
    {
        ColumnGetAttributeInfo(&typinf->cols[i],   typinf->nb_cols, i, &size1, &align1);
        ColumnGetAttributeInfo(&typinf->cols[i+1], typinf->nb_cols, i+1, &size2, &align2);

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
 * ObjectGetAttributeInfo
 * --------------------------------------------------------------------------------------------- */

boolean ObjectGetAttributeInfo
(
    OCI_TypeInfo *typinf,
    int           index,
    size_t       *p_size,
    size_t       *p_align
)
{
    CHECK(typinf  == NULL, 0);
    CHECK(p_size  == NULL, 0);
    CHECK(p_align == NULL, 0);


    if (index >= typinf->nb_cols)
    {
        *p_size = 0;

        return FALSE;
    }

    switch (typinf->cols[index].datatype)
    {
        case OCI_CDT_NUMERIC:
        {
            const ub4 subtype = typinf->cols[index].subtype;

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
            ObjectGetStructSize(typinf->cols[index].typinf, p_size, p_align);
            break;
        }
        default:
        {
            *p_size  = sizeof(void *);
            *p_align = *p_size;
            break;
        }
    }

    if (*p_align > typinf->align)
    {
        typinf->align = *p_align;
    }


    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectReset
 * --------------------------------------------------------------------------------------------- */

void ObjectReset
(
    OCI_Object *obj
)
{
    if (!obj)
    {
        return;
    }
   
    for (ub2 i = 0; i < obj->typinf->nb_cols; i++)
    {
        if (obj->objs[i])
        {
            OCI_Datatype * data = (OCI_Datatype *) obj->objs[i];

            if (OCI_OBJECT_FETCHED_CLEAN == data->hstate)
            {
                data->hstate =  OCI_OBJECT_FETCHED_DIRTY;
            }

            FreeObjectFromType(obj->objs[i], obj->typinf->cols[i].datatype);
            
            obj->objs[i] = NULL;
        }

        FREE(obj->tmpbufs[i])

        obj->tmpsizes[i] = 0;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Object * ObjectInitialize
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
    OCI_TypeInfo *real_typinf = NULL;
    
    DECLARE_CTX(TRUE)

    CALL_CONTEXT_FROM_CON(con)

    real_typinf = ObjectGetRealTypeInfo(typinf, handle);
    STATUS = (NULL != real_typinf);

    ALLOC_DATA(OCI_IPC_OBJECT, obj, 1);

    if (STATUS)
    {      
        obj->con    = con;
        obj->handle = handle;
        obj->typinf = real_typinf;

        if (real_typinf != typinf)
        {
            FREE(obj->objs)
            FREE(obj->tmpbufs)
            FREE(obj->tmpsizes)
        }

        ALLOC_DATA(OCI_IPC_BUFF_ARRAY, obj->tmpbufs, obj->typinf->nb_cols)
        ALLOC_DATA(OCI_IPC_BUFF_ARRAY, obj->tmpsizes, obj->typinf->nb_cols)
        ALLOC_DATA(OCI_IPC_BUFF_ARRAY, obj->objs, obj->typinf->nb_cols)

        ObjectReset(obj);

        if (STATUS && (!obj->handle || (OCI_OBJECT_ALLOCATED_ARRAY == obj->hstate)))
        {
            /* allocates handle for non fetched object */

            if (OCI_OBJECT_ALLOCATED_ARRAY != obj->hstate)
            {
                obj->hstate = OCI_OBJECT_ALLOCATED;
            }

            EXEC
            (
                MemoryAllocateObject(obj->con->env,  obj->con->err, obj->con->cxt,
                            (OCITypeCode) obj->typinf->typecode, obj->typinf->tdo, (dvoid *) NULL,
                            (OCIDuration) OCI_DURATION_SESSION, (boolean) TRUE,
                            (dvoid **) &obj->handle)
            )
        }
        else
        {
            obj->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }

        if (STATUS && (OCI_UNKNOWN == obj->type))
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

        if (STATUS && (reset || !obj->tab_ind))
        {
            if (!parent)
            {
                EXEC
                (
                    OCIObjectGetInd(obj->con->env, obj->con->err,
                                    (dvoid *) obj->handle,
                                    (dvoid **) &obj->tab_ind)
                )
            }
            else
            {
                obj->tab_ind = parent->tab_ind;
                obj->idx_ind = parent->idx_ind + ObjectGetIndOffset(parent->typinf, index);
            }
        }
    }

    /* check for failure */

    if (!STATUS && obj)
    {
        ObjectFree(obj);
        obj = NULL;
    }

    return obj;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetAttributeIndex
 * --------------------------------------------------------------------------------------------- */

int ObjectGetAttributeIndex
(
    OCI_Object  *obj,
    const otext *attr,
    int          type,
    boolean      check
)
{
    int res = -1;

    CHECK(obj  == NULL, res)
    CHECK(attr == NULL, res);

    for (ub2 i = 0; i < obj->typinf->nb_cols; i++)
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
        ExceptionAttributeNotFound(obj->con, attr);
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetAttr
 * --------------------------------------------------------------------------------------------- */

void * ObjectGetAttr
(
    OCI_Object  *obj,
    unsigned int index,
    OCIInd     **pind
)
{
    size_t offset = 0;
    size_t size   = 0;
    size_t align  = 0;

    CHECK(obj  == NULL, NULL)
    CHECK(pind == NULL, NULL)

    if (obj->typinf->struct_size == 0)
    {
        ObjectGetStructSize(obj->typinf, &size, &align);
    }

    offset = (size_t) obj->typinf->offsets[index];

    if (pind)
    {
        const int ind_index = obj->idx_ind + ObjectGetIndOffset(obj->typinf, index);

        *pind = &obj->tab_ind[ind_index];
    }

    return ((char *) obj->handle + offset);
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetNumberInternal
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetNumberInternal
(
    OCI_Object  *obj,
    const otext *attr,
    void        *value,
    uword        flag
)
{
    int index   = 0;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    CALL_CONTEXT_FROM_CON(obj->con)

    STATUS = FALSE;

    index = ObjectGetAttributeIndex(obj, attr, OCI_CDT_NUMERIC, TRUE);

    if (index >= 0)
    {
        OCIInd     *ind = NULL;
        void       *num = ObjectGetAttr(obj, index, &ind);
        OCI_Column *col = &obj->typinf->cols[index];

        STATUS = NumberTranslateValue(obj->con, value, flag, num, col->subtype);

        if (STATUS)
        {
            *ind = OCI_IND_NOTNULL;
        }
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetNumberInternal
 * --------------------------------------------------------------------------------------------- */

boolean ObjectGetNumberInternal
(
    OCI_Object  *obj,
    const otext *attr,
    void        *value,
    uword        flag
)
{
    int index   = 0;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    CALL_CONTEXT_FROM_CON(obj->con)

    STATUS = FALSE;

    index = ObjectGetAttributeIndex(obj, attr, OCI_CDT_NUMERIC, FALSE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        void   *ptr = NULL;

        ptr = ObjectGetAttr(obj, index, &ind);

        if (ptr && (OCI_IND_NULL != *ind))
        {
            OCI_Column *col = &obj->typinf->cols[index];

            STATUS = NumberTranslateValue(obj->con, ptr, col->subtype, value, flag);
        }
    }
    else
    {
        index = ObjectGetAttributeIndex(obj, attr, OCI_CDT_TEXT, FALSE);

        if (index >= 0)
        {
            STATUS = NumberFromStringInternal(obj->con, value, flag, ObjectGetString(obj, attr), NULL);
        }
    }

    if (index == -1)
    {
        ExceptionAttributeNotFound(obj->con, attr);
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Object * ObjectCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf
)
{
    CALL_ENTER(OCI_Object *, NULL)
    CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CALL_CONTEXT_FROM_CON(con)

    RETVAL = ObjectInitialize(con, NULL, NULL, typinf, NULL, -1, TRUE);
    STATUS = (NULL != RETVAL);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectFree
 * --------------------------------------------------------------------------------------------- */

boolean ObjectFree
(
    OCI_Object *obj
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_OBJECT_FETCHED(obj)
    CALL_CONTEXT_FROM_CON(obj->con)

    /* if the object has sub-objects that have been fetched, we need to free
       these objects */

    ObjectReset(obj);

    FREE(obj->objs)
    FREE(obj->tmpbufs)
    FREE(obj->tmpsizes)

    if ((OCI_OBJECT_ALLOCATED == obj->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == obj->hstate))
    {
        MemoryFreeObject(obj->con->env, obj->con->err, obj->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != obj->hstate)
    {
        FREE(obj)
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Object ** ObjectCreateArray
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    CALL_ENTER(OCI_Object **, NULL)
    CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CALL_CONTEXT_FROM_CON(con)

    arr = ArrayCreate(con, nbelem, OCI_CDT_OBJECT, 0, sizeof(void *), sizeof(OCI_Object), 0, typinf);
    STATUS = (NULL != arr);

    if (STATUS)
    {
        RETVAL = (OCI_Object **) arr->tab_obj;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectFreeArray
 * --------------------------------------------------------------------------------------------- */

boolean ObjectFreeArray
(
    OCI_Object **objs
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ARRAY, objs)

    RETVAL = STATUS = ArrayFreeFromHandles((void **)objs);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectAssign
 * --------------------------------------------------------------------------------------------- */

boolean ObjectAssign
(
    OCI_Object *obj,
    OCI_Object *obj_src
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj_src);
    CALL_CHECK_COMPAT(obj->con, obj->typinf->tdo == obj_src->typinf->tdo)
    CALL_CONTEXT_FROM_CON(obj->con)

    EXEC
    (
        OCIObjectCopy(obj->con->env, obj->con->err, obj->con->cxt,
                      obj_src->handle, (obj_src->tab_ind + obj_src->idx_ind),
                      obj->handle, (obj->tab_ind + obj->idx_ind),
                      obj->typinf->tdo, OCI_DURATION_SESSION, OCI_DEFAULT)
    )

    if (STATUS)
    {
        obj->typinf = obj_src->typinf;

        ObjectReset(obj);
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetShort
 * --------------------------------------------------------------------------------------------- */

boolean ObjectGetBoolean
(
    OCI_Object  *obj,
    const otext *attr
)
{
    int index = -1;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    CALL_CONTEXT_FROM_CON(obj->con)

    STATUS = FALSE;

    index = ObjectGetAttributeIndex(obj, attr, OCI_CDT_BOOLEAN, TRUE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        boolean *value = NULL;

        STATUS = TRUE;

        value = (boolean *)ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            RETVAL = *value;
        }
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetNumber
 * --------------------------------------------------------------------------------------------- */

OCI_Number * ObjectGetNumber
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_VALUE
    (
        OCI_CDT_NUMERIC,
        OCI_Number*,
        OCINumber*,
        NumberInitialize(obj->con, (OCI_Number *) obj->objs[index], (OCINumber *) value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetShort
 * --------------------------------------------------------------------------------------------- */

short ObjectGetShort
(
    OCI_Object  *obj,
    const otext *attr
)
{
    short value = 0;

    ObjectGetNumberInternal(obj, attr, &value, OCI_NUM_SHORT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

unsigned short ObjectGetUnsignedShort
(
    OCI_Object  *obj,
    const otext *attr
)
{
    unsigned short value = 0;

    ObjectGetNumberInternal(obj, attr, &value, OCI_NUM_USHORT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetInt
 * --------------------------------------------------------------------------------------------- */

int ObjectGetInt
(
    OCI_Object  *obj,
    const otext *attr
)
{
    int value = 0;

    ObjectGetNumberInternal(obj, attr, &value, OCI_NUM_INT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

unsigned int ObjectGetUnsignedInt
(
    OCI_Object  *obj,
    const otext *attr
)
{
    unsigned int value = 0;

    ObjectGetNumberInternal(obj, attr, &value, OCI_NUM_UINT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetBigInt
 * --------------------------------------------------------------------------------------------- */

big_int ObjectGetBigInt
(
    OCI_Object  *obj,
    const otext *attr
)
{
    big_int value = 0;

    ObjectGetNumberInternal(obj, attr, &value, OCI_NUM_BIGINT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

big_uint ObjectGetUnsignedBigInt
(
    OCI_Object  *obj,
    const otext *attr
)
{
    big_uint value = 0;

    ObjectGetNumberInternal(obj, attr, &value, OCI_NUM_BIGUINT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetDouble
 * --------------------------------------------------------------------------------------------- */

double ObjectGetDouble
(
    OCI_Object  *obj,
    const otext *attr
)
{
    double value = 0.0;

    ObjectGetNumberInternal(obj, attr, &value, OCI_NUM_DOUBLE);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetFloat
 * --------------------------------------------------------------------------------------------- */

float ObjectGetFloat
(
    OCI_Object  *obj,
    const otext *attr
)
{
    float value = 0.0f;

    ObjectGetNumberInternal(obj, attr, &value, OCI_NUM_FLOAT);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetString
 * --------------------------------------------------------------------------------------------- */

const otext * ObjectGetString
(
    OCI_Object  *obj,
    const otext *attr
)
{
    int index = -1;

    CALL_ENTER(const otext *, NULL)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    CALL_CONTEXT_FROM_CON(obj->con)

    STATUS = FALSE;

    index = ObjectGetAttributeIndex(obj, attr, OCI_CDT_TEXT, FALSE);

    if (index >= 0)
    {
        OCIInd *ind       = NULL;
        OCIString **value = NULL;

        STATUS = TRUE;

        value = (OCIString **) ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            if (Env.use_wide_char_conv)
            {
                RETVAL = StringFromStringPtr(obj->con->env, *value, &obj->tmpbufs[index], &obj->tmpsizes[index]);
            }
            else
            {
                RETVAL = (otext *)OCIStringPtr(obj->con->env, *value);
            }
        }
    }
    else
    {
        index = ObjectGetAttributeIndex(obj, attr, -1, FALSE);

        if (index >= 0)
        {
            OCI_Error   *err   = ErrorGet(TRUE, TRUE);
            OCIInd      *ind   = NULL;
            void        *value = NULL;
            unsigned int size  = 0;
            unsigned int len   = 0;

            STATUS = TRUE;

            value = ObjectGetAttr(obj, index, &ind);

            /* special case of RAW attribute, we need their length */
            if (OCI_CDT_RAW == obj->typinf->cols[index].datatype)
            {
                if (value && ind && (OCI_IND_NULL != *ind))
                {
                    size  = OCIRawSize(obj->con->env, (*(OCIRaw **) value));
                    value = OCIRawPtr(obj->con->env,  (*(OCIRaw **) value));
                }
            }

            len = StringGetFromType(obj->con, &obj->typinf->cols[index], value, size, NULL, 0, FALSE);
            STATUS = (NULL == err || OCI_UNKNOWN == err->type);

            if (STATUS && len > 0)
            {
                STATUS = StringRequestBuffer(&obj->tmpbufs[index], &obj->tmpsizes[index], len);

                if (STATUS)
                {
                    const unsigned int real_tmpsize = StringGetFromType(obj->con, &obj->typinf->cols[index], value, size, obj->tmpbufs[index], obj->tmpsizes[index], FALSE);
                
                    STATUS = (NULL == err || OCI_UNKNOWN == err->type);

                    if (STATUS && real_tmpsize > 0)
                    {
                        RETVAL = obj->tmpbufs[index];
                    }
                }
            }
        }
    }

    if (index == -1)
    {
        ExceptionAttributeNotFound(obj->con, attr);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetRaw
 * --------------------------------------------------------------------------------------------- */

int ObjectGetRaw
(
    OCI_Object  *obj,
    const otext *attr,
    void        *buffer,
    unsigned int len
)
{
    int index = -1;

    CALL_ENTER(int, 0);
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    CALL_CONTEXT_FROM_CON(obj->con)

    STATUS = FALSE;

    index = ObjectGetAttributeIndex(obj, attr, OCI_CDT_RAW, TRUE);

    if (index >= 0)
    {
        OCIInd *ind    = NULL;
        OCIRaw **value = NULL;

        STATUS = TRUE;

        value = (OCIRaw **) ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            const ub4 raw_len = OCIRawSize(obj->con->env, *value);

            if (len > raw_len)
            {
                len = raw_len;
            }

            memcpy(buffer, OCIRawPtr(obj->con->env, *value), (size_t) len);

            RETVAL = (int) len;
        }
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
* ObjectGetRawSize
* --------------------------------------------------------------------------------------------- */

unsigned int ObjectGetRawSize
(
    OCI_Object  *obj,
    const otext *attr
)
{
    ub4 raw_len = 0;
    int index = -1;

    CALL_ENTER(unsigned int, 0)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    CALL_CONTEXT_FROM_CON(obj->con)

    STATUS = FALSE;

    index = ObjectGetAttributeIndex(obj, attr, OCI_CDT_RAW, TRUE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        OCIRaw **value = NULL;

        STATUS = TRUE;

        value = (OCIRaw **)ObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            raw_len = OCIRawSize(obj->con->env, *value);
        }
    }

    RETVAL = raw_len;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * ObjectGetDate
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_VALUE
    (
        OCI_CDT_DATETIME,
        OCI_Date*,
        OCIDate,
        DateInitialize(obj->con, (OCI_Date *) obj->objs[index], value, FALSE, FALSE)
    )
 }

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * ObjectGetTimestamp
(
    OCI_Object  *obj,
    const otext *attr
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    OBJECT_GET_VALUE
    (
        OCI_CDT_TIMESTAMP,
        OCI_Timestamp*,
        OCIDateTime*,
        TimestampInitialize(obj->con, (OCI_Timestamp *) obj->objs[index],
                     (OCIDateTime *) *value, obj->typinf->cols[index].subtype)
    )

#else

    CALL_ENTER( OCI_Timestamp *, NULL)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_EXIT()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * ObjectGetInterval
(
    OCI_Object  *obj,
    const otext *attr
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    OBJECT_GET_VALUE
        (
        OCI_CDT_INTERVAL,
        OCI_Interval*,
        OCIInterval *,
        IntervalInitialize(obj->con, (OCI_Interval *) obj->objs[index],
                     (OCIInterval *) *value, obj->typinf->cols[index].subtype)
    )

#else

    CALL_ENTER(OCI_Interval *, NULL)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_EXIT()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetColl
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * ObjectGetColl
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCI_Coll*,
        OCIColl*,
        CollectionInitialize(obj->con, (OCI_Coll *) obj->objs[index], (OCIColl *) *value, obj->typinf->cols[index].typinf)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * ObjectGetObject
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_VALUE
    (
        OCI_CDT_OBJECT,
        OCI_Object*,
        void,
        ObjectInitialize(obj->con, (OCI_Object *) obj->objs[index], value,
                   obj->typinf->cols[index].typinf,  obj, index, FALSE)
    )
 }

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * ObjectGetLob
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_VALUE
    (
        OCI_CDT_LOB,
        OCI_Lob*,
        OCILobLocator*,
        LobInitialize(obj->con, (OCI_Lob *) obj->objs[index], *value, obj->typinf->cols[index].subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * ObjectGetFile
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_VALUE
    (
        OCI_CDT_FILE,
        OCI_File *,
        OCILobLocator*,
        FileInitialize(obj->con, (OCI_File *) obj->objs[index], *value, obj->typinf->cols[index].subtype)
    )
 }

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetReference
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * ObjectGetReference
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_VALUE
    (
        OCI_CDT_REF,
        OCI_Ref*,
        OCIRef*,
        ReferenceInitialize(obj->con, NULL, (OCI_Ref *) obj->objs[index], *value)
    )
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ObjectSetBoolean
* --------------------------------------------------------------------------------------------- */

boolean ObjectSetBoolean
(
    OCI_Object  *obj,
    const otext *attr,
    boolean      value
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    CALL_CONTEXT_FROM_CON(obj->con)

    STATUS = FALSE;

    const int index = ObjectGetAttributeIndex(obj, attr, OCI_CDT_BOOLEAN, TRUE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        boolean *data = (boolean *)ObjectGetAttr(obj, index, &ind);

        if (data)
        {
            *data = value;
            *ind = OCI_IND_NOTNULL;

            STATUS = TRUE;
        }
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetNumber
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetNumber
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Number  *value
)
{
    OBJECT_SET_VALUE
    (
        OCI_CDT_NUMERIC,
        OCINumber,
        OCINumberAssign( obj->con->err, value->handle, data)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetShort
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetShort
(
    OCI_Object  *obj,
    const otext *attr,
    short        value
)
{
    return ObjectSetNumberInternal(obj, attr, &value, (uword) OCI_NUM_SHORT);
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetUnsignedShort
(
    OCI_Object    *obj,
    const otext   *attr,
    unsigned short value
)
{
    return ObjectSetNumberInternal(obj, attr, &value, (uword) OCI_NUM_USHORT);
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetInt
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetInt
(
    OCI_Object  *obj,
    const otext *attr,
    int          value
)
{
    return ObjectSetNumberInternal(obj, attr, &value, (uword) OCI_NUM_INT);
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetUnsignedInt
(
    OCI_Object  *obj,
    const otext *attr,
    unsigned int value
)
{
    return ObjectSetNumberInternal(obj, attr, &value, (uword) OCI_NUM_UINT);
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetBigInt
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetBigInt
(
    OCI_Object  *obj,
    const otext *attr,
    big_int      value
)
{
    return ObjectSetNumberInternal(obj, attr, &value, (uword) OCI_NUM_BIGINT);
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetUnsignedBigInt
(
    OCI_Object  *obj,
    const otext *attr,
    big_uint     value
)
{
    return ObjectSetNumberInternal(obj, attr, &value, (uword) OCI_NUM_BIGUINT);
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetDouble
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetDouble
(
    OCI_Object  *obj,
    const otext *attr,
    double       value
)
{
    return ObjectSetNumberInternal(obj, attr, &value, (uword) OCI_NUM_DOUBLE);
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetFloat
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetFloat
(
    OCI_Object  *obj,
    const otext *attr,
    float        value
)
{
    return ObjectSetNumberInternal(obj, attr, &value, (uword) OCI_NUM_FLOAT);
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetString
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetString
(
    OCI_Object  *obj,
    const otext *attr,
    const otext *value
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    CALL_CONTEXT_FROM_CON(obj->con)

    STATUS = FALSE;

    if (!value)
    {
        STATUS = ObjectSetNull(obj, attr);
    }
    else
    {
        const int index = ObjectGetAttributeIndex(obj, attr, OCI_CDT_TEXT, TRUE);

        if (index >= 0)
        {
            OCIInd *ind      = NULL;
            OCIString **data = (OCIString **) ObjectGetAttr(obj, index, &ind);

            STATUS = StringToStringPtr(obj->con->env, data, obj->con->err, value);

            if (STATUS)
            {
                *ind = OCI_IND_NOTNULL;
            }
        }
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetRaw
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetRaw
(
    OCI_Object  *obj,
    const otext *attr,
    void       * value,
    unsigned int len
)
{
    OBJECT_SET_VALUE
    (
        OCI_CDT_RAW,
        OCIRaw*,
        OCIRawAssignBytes(obj->con->env, obj->con->err, (ub1*) value, len, data)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetDate
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetDate
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Date    *value
)
{
    OBJECT_SET_VALUE
    (
        OCI_CDT_DATETIME,
        OCIDate,
        OCIDateAssign(obj->con->err, value->handle, data)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetTimestamp
(
    OCI_Object    *obj,
    const otext   *attr,
    OCI_Timestamp *value
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    OBJECT_SET_VALUE
    (
        OCI_CDT_TIMESTAMP,
        OCIDateTime*,
        OCIDateTimeAssign((dvoid *) obj->con->env, obj->con->err, value->handle, *data)
    )

#else

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_EXIT()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetInterval
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetInterval
(
    OCI_Object   *obj,
    const otext  *attr,
    OCI_Interval *value
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    OBJECT_SET_VALUE
    (
        OCI_CDT_INTERVAL,
        OCIInterval*,
        OCIIntervalAssign((dvoid *) obj->con->env, obj->con->err, value->handle, *data)
    )

#else

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_EXIT()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetColl
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetColl
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Coll    *value
)
{
    OBJECT_SET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCIColl*,
        OCICollAssign(obj->con->env, obj->con->err, value->handle, *data)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetObject
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetObject
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Object  *value
)
{
    OBJECT_SET_VALUE
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
 * ObjectSetLob
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetLob
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Lob     *value
)
{
    OBJECT_SET_VALUE
    (
        OCI_CDT_LOB,
        OCILobLocator*,
        OCILobLocatorAssign(obj->con->cxt, obj->con->err, value->handle, (OCILobLocator **) data)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetFile
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetFile
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_File    *value
)
{
    OBJECT_SET_VALUE
    (
        OCI_CDT_FILE,
        OCILobLocator*,
        OCILobLocatorAssign(obj->con->cxt, obj->con->err, value->handle, (OCILobLocator **) data)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetReference
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetReference
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Ref     *value
)
{
    OBJECT_SET_VALUE
    (
        OCI_CDT_REF,
        OCIRef*,
        OCIRefAssign(obj->con->env, obj->con->err, value->handle, data)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectSetNull
 * --------------------------------------------------------------------------------------------- */

boolean ObjectSetNull
(
    OCI_Object  *obj,
    const otext *attr
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    CALL_CONTEXT_FROM_CON(obj->con)

    const int index = ObjectGetAttributeIndex(obj, attr, -1, TRUE);

    if (index >= 0)
    {
        const int ind_index = obj->idx_ind + ObjectGetIndOffset(obj->typinf, index);

        obj->tab_ind[ind_index] = OCI_IND_NULL;

        STATUS = TRUE;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectIsNull
 * --------------------------------------------------------------------------------------------- */

boolean ObjectIsNull
(
    OCI_Object  *obj,
    const otext *attr
)
{
    int index   = 0;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_STRING, attr)
    CALL_CONTEXT_FROM_CON(obj->con)

    index = ObjectGetAttributeIndex(obj, attr, -1, TRUE);

    if (index >= 0)
    {
        const int ind_index = obj->idx_ind + ObjectGetIndOffset(obj->typinf, index);

        RETVAL = (OCI_IND_NOTNULL != obj->tab_ind[ind_index]);

        STATUS = TRUE;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * ObjectGetTypeInfo
(
    OCI_Object *obj
)
{
    GET_PROP(OCI_TypeInfo*, NULL, OCI_IPC_OBJECT, obj, typinf, obj->con, NULL, obj->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int ObjectGetType
(
    OCI_Object *obj
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_OBJECT, obj, type, obj->con, NULL, obj->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetSelfRef
 * --------------------------------------------------------------------------------------------- */

boolean ObjectGetSelfRef
(
    OCI_Object *obj,
    OCI_Ref    *ref
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_REF, ref)
    CALL_CHECK_COMPAT(obj->con, obj->typinf->tdo == ref->typinf->tdo)
    CALL_CONTEXT_FROM_CON(obj->con)

    EXEC(OCIObjectGetObjectRef(obj->con->env, obj->con->err, obj->handle, ref->handle))

    if (!STATUS && ref->obj)
    {
        ObjectFree(ref->obj);
        ref->obj = NULL;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectGetStruct
 * --------------------------------------------------------------------------------------------- */

boolean ObjectGetStruct
(
    OCI_Object *obj,
    void      **pp_struct,
    void      **pp_ind
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CONTEXT_FROM_CON(obj->con)

    if (pp_struct)
    {
        *pp_struct = (void *) obj->handle;
    }

    if (pp_ind)
    {
        *pp_ind = (void *) obj->tab_ind;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ObjectToString
 * --------------------------------------------------------------------------------------------- */

boolean ObjectToString
(
    OCI_Object   *obj,
    unsigned int *size,
    otext        *str
)
{
    OCI_Error   *err   = NULL;
    otext       *attr  = NULL;
    boolean      quote = TRUE;
    unsigned int len   = 0;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_OBJECT, obj)
    CALL_CHECK_PTR(OCI_IPC_VOID, size)
    CALL_CONTEXT_FROM_CON(obj->con)

    err = ErrorGet(TRUE, TRUE);

    if (str)
    {
        *str = 0;
    }

    len += StringAddToBuffer(str, len, obj->typinf->name, (unsigned int) ostrlen(obj->typinf->name), FALSE);
    len += StringAddToBuffer(str, len, OTEXT("("), 1, FALSE);

    for (int i = 0; i < obj->typinf->nb_cols && STATUS; i++)
    {
        attr  = obj->typinf->cols[i].name;
        quote = TRUE;

        if (ObjectIsNull(obj, attr))
        {
            len += StringAddToBuffer(str, len, OCI_STRING_NULL, OCI_STRING_NULL_SIZE, FALSE);
        }
        else
        {
            void *data = NULL;
            unsigned int data_size = 0;
            const unsigned int data_type = obj->typinf->cols[i].datatype;

            switch (data_type)
            {
                case OCI_CDT_TEXT:
                {
                    OCIInd *ind = NULL;
                    data = ObjectGetAttr(obj, i, &ind);

                    if (data && ind && (OCI_IND_NULL != *ind))
                    {
                        data_size = OCIStringSize(Env.env, (*(OCIString **)data));
                        data      = (void *)ObjectGetString(obj, attr);
                    }
                    else
                    {
                        data = NULL;
                    }
                    break;
                }
                case OCI_CDT_BOOLEAN:
                {
                    OCIInd *ind = NULL;
                    data = ObjectGetAttr(obj, i, &ind);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_NUMERIC:
                {
                    OCIInd *ind = NULL;
                    data  = ObjectGetAttr(obj, i, &ind);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_RAW:
                {
                    OCIInd *ind = NULL;
                    data = ObjectGetAttr(obj, i, &ind);

                    if (data && ind && (OCI_IND_NULL != *ind))
                    {
                        data_size = OCIRawSize(obj->con->env, (*(OCIRaw **) data));
                        data      = OCIRawPtr(obj->con->env,  (*(OCIRaw **) data));
                    }
                    else
                    {
                        data = NULL;
                    }                
                    break;
                }
                case OCI_CDT_DATETIME:
                {
                    data  = (void *) ObjectGetDate(obj, attr);
                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    data  = (void *) ObjectGetTimestamp(obj, attr);
                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    data  = (void *) ObjectGetInterval(obj, attr);
                    break;
                }
                case OCI_CDT_LOB:
                {
                    data  = (void *) ObjectGetLob(obj, attr);
                    break;
                }
                case OCI_CDT_FILE:
                {
                    data  = (void *) ObjectGetFile(obj, attr);
                    break;
                }
                case OCI_CDT_REF:
                {
                    data  = (void *) ObjectGetReference(obj, attr);
                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    data  = (void *) ObjectGetObject(obj, attr);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    data =  (void *) ObjectGetColl(obj, attr);
                    quote = FALSE;
                }
            }

            STATUS = (NULL != data || OCI_CDT_TEXT == data_type) && (NULL == err || !err->raise);

            if (STATUS)
            {
                otext *tmpbuf = str;

                if (tmpbuf)
                {
                    tmpbuf += len;
                }

                if (data)
                {
                    len += StringGetFromType(obj->con, &obj->typinf->cols[i], data, data_size, tmpbuf, tmpbuf && size ? *size - len : 0, quote);
                }
                else
                {
                    len += StringAddToBuffer(str, len, OCI_STRING_NULL, OCI_STRING_NULL_SIZE, FALSE);
                }

                STATUS = (NULL == err || OCI_UNKNOWN == err->type);
            }
        }

        if (STATUS && i < (obj->typinf->nb_cols-1))
        {
            len += StringAddToBuffer(str, len, OTEXT(", "), 2, quote);
        }
    }

    if (STATUS)
    {
        len += StringAddToBuffer(str, len, OTEXT(")"), 1, FALSE);

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

    RETVAL = STATUS;

    CALL_EXIT()
}

