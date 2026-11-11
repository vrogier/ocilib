/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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
#include "stringutils.h"
#include "timestamp.h"
#include "typeinfo.h"

#define OBJECT_SET_VALUE(datatype, type, func, ...)                                     \
                                                                                        \
        ENTER_FUNC(OCI_Boolean, FALSE, OCI_IPC_OBJECT, obj);                            \
                                                                                        \
        CHECK_PTR(OCI_IPC_OBJECT, obj);                                                 \
                                                                                        \
        if (NULL == value)                                                              \
        {                                                                               \
            CHECK(OcilibObjectSetNull(obj, attr));                                      \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            OCI_Int32 index = OcilibObjectGetAttributeIndex(obj, attr, datatype, TRUE); \
                                                                                        \
            if (index >= 0)                                                             \
            {                                                                           \
                OCIInd *ind  = NULL;                                                    \
                type   *data = (type *) OcilibObjectGetAttr(obj, index, &ind);          \
                                                                                        \
                CHECK_OCI(obj->con->err, func, __VA_ARGS__);                            \
                                                                                        \
                *ind = OCI_IND_NOTNULL;                                                 \
            }                                                                           \
        }                                                                               \
                                                                                        \
        SET_SUCCESS();                                                                  \
                                                                                        \
        EXIT_FUNC();

#define OBJECT_GET_VALUE(datatype, object_type, type, func, ...)          \
                                                                          \
        ENTER_FUNC(object_type, NULL, OCI_IPC_OBJECT, obj);               \
                                                                          \
        OCI_Int32 index = 0;                                              \
                                                                          \
        CHECK_PTR(OCI_IPC_OBJECT, obj);                                   \
                                                                          \
        index = OcilibObjectGetAttributeIndex(obj, attr, datatype, TRUE); \
        CHECK(index >= 0);                                                \
                                                                          \
        OCIInd *ind   = NULL;                                             \
        type   *value = NULL;                                             \
                                                                          \
        value = (type *) OcilibObjectGetAttr(obj, index, &ind);           \
                                                                          \
        object_type tmp = NULL;                                           \
                                                                          \
        if (value && ind && (OCI_IND_NULL != *ind))                       \
        {                                                                 \
            tmp = obj->objs[index] = func(__VA_ARGS__);                   \
            CHECK_NULL(tmp);                                              \
        }                                                                 \
                                                                          \
        SET_RETVAL(tmp);                                                  \
                                                                          \
        EXIT_FUNC();

#define OBJECT_GET_NUMBER(number_type, type, value)                  \
                                                                     \
        ENTER_FUNC(type, value, OCI_IPC_OBJECT, obj);                \
                                                                     \
        CHECK_PTR(OCI_IPC_OBJECT, obj);                              \
                                                                     \
        type tmp = value;                                            \
                                                                     \
        CHECK(OcilibObjectGetNumberInternal(obj, attr,               \
                                            (void *) (&tmp),         \
                                            (uword) (number_type))); \
                                                                     \
        SET_RETVAL(tmp);                                             \
                                                                     \
        EXIT_FUNC();

#define OBJECT_SET_NUMBER(number_type, value)                             \
                                                                          \
        ENTER_FUNC(OCI_Boolean, FALSE,OCI_IPC_OBJECT, obj);               \
                                                                          \
        CHECK_PTR(OCI_IPC_OBJECT, obj);                                   \
                                                                          \
        CHECK(OcilibObjectSetNumberInternal(obj, attr, (void *) &(value), \
                                            (uword) (number_type)));      \
                                                                          \
        SET_SUCCESS();                                                    \
                                                                          \
        EXIT_FUNC()

/* pre declaration */

static OCI_Boolean OcilibObjectGetAttributeInfo
(
    OCI_TypeInfo * typinf,
    OCI_Int32      index,
    OCI_Size     * p_size,
    OCI_Size     * p_align
);

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectTypeInfoFind
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibObjectTypeInfoFind(OCI_TypeInfo * typinf, OCIType* tdo)
{
    return NULL != typinf && typinf->handle == tdo;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetRealTypeInfo
 * --------------------------------------------------------------------------------------------- */

static OCI_TypeInfo * ObjectGetRealTypeInfo(OCI_TypeInfo *typinf, void *object)
{
    ENTER_FUNC
    (
        /* returns */ OCI_TypeInfo*, typinf,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    OCIRef      * ref    = NULL;
    OCIType     * tdo    = NULL;
    OCIDescribe * descr  = NULL;
    OCI_TypeInfo* parent = NULL;

    otext* schema_name = NULL;
    otext* object_name = NULL;

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);

    CHECK_NULL(object);

    /* nothing to do if the type is final */
    CHECK(typinf->type == OCI_TIF_TYPE && !typinf->is_final);

    /* if the type is related to UTDs and is virtual (e.g. non final), we must find the real type of
    * the instance */

    /* create a local REF to store a REF to the object real type */

    CHECK(OcilibMemoryAllocateObject(typinf->con->env, typinf->con->env->handle, typinf->con->err,
                                     typinf->con->handle, SQLT_REF, (OCIType *)0,
                                     NULL, OCI_DURATION_SESSION, 0, (void**)&ref));

    CHECK_OCI
    (
        typinf->con->err,
        OCIObjectGetTypeRef,
        typinf->con->env->handle,
        typinf->con->err,
        (dvoid*)object, ref
    );

    CHECK_OCI
    (
        typinf->con->err,
        OCITypeByRef, typinf->con->env->handle,
        typinf->con->err, ref,
        OCI_DURATION_SESSION, OCI_TYPEGET_HEADER,
        &tdo
    );

    CHECK_NULL(tdo);

    /* the object instance type pointer is different only if the instance is from an inherited  type
     * */

    CHECK(tdo != typinf->handle);

    /* first try to find it in list */

    LIST_ATOMIC_FIND(typinf->con->tinfs, OcilibObjectTypeInfoFind, tdo, parent);

    /* if found, it will be assigned in the cleanup part */

    CHECK(parent == NULL);

    OCIParam *param = NULL;

    OCI_UInt32 size_schema = 0;
    OCI_UInt32 size_object = 0;

    otext fullname[(OCI_SIZE_OBJ_NAME * 2) + 2] = OTEXT("");

    CHECK(OcilibMemoryAllocHandle(typinf->con->env, typinf->con->env->handle, (void**) &descr, OCI_HTYPE_DESCRIBE));

    CHECK_OCI
    (
        typinf->con->err,
        OCIDescribeAny,
        typinf->con->handle, typinf->con->err,
        (dvoid *)tdo, 0, OCI_OTYPE_PTR, OCI_DEFAULT,
        OCI_PTYPE_UNK, descr
    );

    CHECK_ATTRIB_GET
    (
        OCI_HTYPE_DESCRIBE, OCI_ATTR_PARAM,
        descr, &param, NULL,
        typinf->con->err
    );

    CHECK(OcilibStringGetAttribute(typinf->con, param, OCI_DTYPE_PARAM,
                                   OCI_ATTR_SCHEMA_NAME, &schema_name, &size_schema));

    CHECK(OcilibStringGetAttribute(typinf->con, param, OCI_DTYPE_PARAM,
                                   OCI_ATTR_NAME, &object_name, &size_object));

    /* compute link full name */

    OcilibStringGetFullTypeName(schema_name, NULL, object_name, NULL,
                                fullname, (sizeof(fullname) / sizeof(otext)) - 1);

    /* retrieve the type info of the real object */
    parent = OcilibTypeInfoGet(typinf->con, fullname, OCI_TIF_TYPE);

    CLEANUP_AND_EXIT_FUNC
    (
        FREE(schema_name);
        FREE(object_name);

        if (NULL != typinf)
        {
            if (NULL != ref)
            {
                OcilibMemoryFreeObject(typinf->con->env, typinf->con->env->handle, typinf->con->err, ref, OCI_DEFAULT);
            }

            if (NULL != descr)
            {
                OcilibMemoryFreeHandle(typinf->con->env, descr, OCI_HTYPE_DESCRIBE);
            }
        }

        SET_RETVAL( NULL != parent ? parent : typinf);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetIndicatorOffset
 * --------------------------------------------------------------------------------------------- */

static ub2 OcilibObjectGetIndOffset
(
    OCI_TypeInfo *typinf,
    OCI_Int32     index
)
{
    OCI_Int32 i = 0;
    ub2       j = 1;

    if (typinf == NULL) return (0);

    for (i = 0; i < index; i++)
    {
        if (OCI_CDT_OBJECT == typinf->cols[i].datatype)
        {
            j += OcilibObjectGetIndOffset(typinf->cols[i].typinf, typinf->cols[i].typinf->nb_cols);
        }
        else
        {
            j++;
        }

    }

    return j;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetStructSize
 * --------------------------------------------------------------------------------------------- */

static void OcilibObjectGetStructSize
(
    OCI_TypeInfo *typinf,
    OCI_Size     *p_size,
    OCI_Size     *p_align
)
{
    if (NULL == typinf || NULL == p_size || NULL == p_align)
    {
        return;
    }

    if (typinf->struct_size == 0)
    {
        OCI_Size size  = 0;
        OCI_Size size1 = 0;
        OCI_Size size2 = 0;
        OCI_Size align = 0;

        ub2 i = 0;

        /* if the type is a sub type, then it is a subset containing all of his parent members
           In that case the first member is a buffer holding a parent object structure */

        if (typinf->parent_type)
        {
            /* if super type information has not been already cached, then let's compute it now */

            OcilibObjectGetStructSize(typinf->parent_type, &size, &align);

            /* copy super type members offsets to the current sub type of members offsets */

            for (; i < typinf->parent_type->nb_cols; i++)
            {
                typinf->offsets[i] = typinf->parent_type->offsets[i];
            }

            /* adjust current member index to start to compute with the first of the derived type */

            i = typinf->parent_type->nb_cols;

            /* compute the first derived member in order to not touch to the next for loop code that
             * is working :) */

            if (i < typinf->nb_cols)
            {
                OCI_Size next_align = 0;

                /* set current alignment to the parent one as it is the first member of the current
                 * structure */

                typinf->align = align;

                /* get current type self first member information (after parent type) */

                OcilibObjectGetAttributeInfo(typinf, i, &size2, &next_align);

                /* make sure that parent field is aligned */

                size = ROUNDUP(size, next_align);
            }
        }

        for (; i < typinf->nb_cols; i++)
        {
            if (i > 0)
            {
                size1 = size2;

                typinf->offsets[i] = (OCI_Int32) size;
            }
            else
            {
                OcilibObjectGetAttributeInfo(typinf, i, &size1, &align);

                typinf->offsets[i] = 0;
            }

            OcilibObjectGetAttributeInfo(typinf, i + 1, &size2, &align);

            size += size1;

            size = ROUNDUP(size, align);
        }

        typinf->struct_size = ROUNDUP(size + size2, typinf->align);
    }

    *p_size  = typinf->struct_size;
    *p_align = typinf->align;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetUserStructSize
 * --------------------------------------------------------------------------------------------- */

void OcilibObjectGetUserStructSize
(
    OCI_TypeInfo* typinf,
    OCI_Size    * p_size,
    OCI_Size    * p_align
)
{
    OCI_Size size1  = 0;
    OCI_Size size2  = 0;
    OCI_Size align1 = 0;
    OCI_Size align2 = 0;
    OCI_Size align  = 0;

    OCI_Size size = 0;

    if (!typinf || !p_size || !p_align)
    {
        return;
    }

    for (ub2 i = 0; i < typinf->nb_cols; i++)
    {
        OcilibColumnGetAttributeInfo(&typinf->cols[i], typinf->nb_cols, i, &size1, &align1);

        if (i + 1 < typinf->nb_cols)
        {
            OcilibColumnGetAttributeInfo(&typinf->cols[i + 1], typinf->nb_cols, i + 1, &size2, &align2);
        }
        else
        {
            size2  = 0;
            align2 = 0;
        }

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
 * OcilibObjectGetAttributeInfo
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibObjectGetAttributeInfo
(
    OCI_TypeInfo *typinf,
    OCI_Int32     index,
    OCI_Size     *p_size,
    OCI_Size     *p_align
)
{
    if (typinf  == NULL) return (0);
    if (p_size  == NULL) return (0);
    if (p_align == NULL) return (0);

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

            if (subtype & OCI_NUM_INT16)
            {
                *p_size  = sizeof(OCI_Int16);
                *p_align = *p_size;
            }
            else if (subtype & OCI_NUM_INT32)
            {
                *p_size  = sizeof(OCI_Int32);
                *p_align = *p_size;
            }
            else if (subtype & OCI_NUM_FLOAT)
            {
                *p_size  = sizeof(OCI_Double);
                *p_align = *p_size;
            }
            else if (subtype & OCI_NUM_DOUBLE)
            {
                *p_size  = sizeof(OCI_Double);
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
            *p_size  = sizeof(OCI_Boolean);
            *p_align = *p_size;
            break;
        }
        case OCI_CDT_OBJECT:
        {
            OcilibObjectGetStructSize(typinf->cols[index].typinf, p_size, p_align);
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
 * OcilibObjectReset
 * --------------------------------------------------------------------------------------------- */

static void OcilibObjectReset
(
    OCI_Object *obj
)
{
    if (NULL == obj)
    {
        return;
    }

    for (ub2 i = 0; i < obj->typinf->nb_cols; i++)
    {
        if (obj->objs[i])
        {
            OCI_DataType* data = (OCI_DataType*) obj->objs[i];

            if (OCI_OBJECT_FETCHED_CLEAN == data->hstate)
            {
                data->hstate =  OCI_OBJECT_FETCHED_DIRTY;
            }

            OcilibFreeObjectFromType(obj->objs[i], obj->typinf->cols[i].datatype);

            obj->objs[i] = NULL;
        }

        OcilibMemoryFree(obj->con->env, obj->tmpbufs[i]);
        obj->tmpbufs[i] = NULL;

        obj->tmpsizes[i] = 0;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OcilibObjectInitialize
(
    OCI_Connection *con,
    OCI_Object     *obj,
    void           *handle,
    OCI_TypeInfo   *typinf,
    OCI_Object     *parent,
    OCI_Int32       index,
    OCI_Boolean     reset
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Object*, obj,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_TypeInfo* real_typinf = ObjectGetRealTypeInfo(typinf, handle);
    CHECK_NULL(real_typinf);

    ALLOC_HANDLE(OCI_IPC_OBJECT, obj);
    obj->con    = con;
    obj->handle = handle;
    obj->typinf = real_typinf;

    if (real_typinf != typinf)
    {
        FREE(obj->objs);
        FREE(obj->tmpbufs);
        FREE(obj->tmpsizes);
    }

    ALLOC_DATA(OCI_IPC_BUFF_ARRAY, obj->tmpbufs,  obj->typinf->nb_cols);
    ALLOC_DATA(OCI_IPC_BUFF_ARRAY, obj->tmpsizes, obj->typinf->nb_cols);
    ALLOC_DATA(OCI_IPC_BUFF_ARRAY, obj->objs,     obj->typinf->nb_cols);

    OcilibObjectReset(obj);

    if (NULL == obj->handle || OCI_OBJECT_ALLOCATED_ARRAY == obj->hstate)
    {
        /* allocates handle for non fetched object */

        if (OCI_OBJECT_ALLOCATED_ARRAY != obj->hstate)
        {
            obj->hstate = OCI_OBJECT_ALLOCATED;
        }

        CHECK(OcilibMemoryAllocateObject(obj->con->env, obj->con->env->handle, obj->con->err, obj->con->handle,
                                         (OCITypeCode)obj->typinf->typecode, obj->typinf->handle,
                                         (dvoid *) NULL, (OCIDuration) OCI_DURATION_SESSION,
                                         (OCI_Boolean) TRUE, (dvoid **) &obj->handle));
    }
    else
    {
        obj->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

    if (OCI_UNKNOWN == obj->type)
    {
        ub4 size = sizeof(obj->type);

        /* calling OCIObjectGetProperty() on objects that are attributes of
           parent objects leads to a segmentation fault on MS Windows !
           We need to report that to Oracle! Because sub objects always are
           values, if the parent indicator array is not null, let's assign
           the object type properties ourselves */

        if (NULL == parent)
        {
            sword ret;

            OCI_CALL(ret, OCIObjectGetProperty, obj->con->env->handle, obj->con->err,
                     obj->handle,
                     (OCIObjectPropId) OCI_OBJECTPROP_LIFETIME,
                     (void *) &obj->type, &size);
        }
        else
        {
            obj->type = OCI_OBJECT_VALUE;
        }
    }

    if (reset || NULL == obj->tab_ind)
    {
        if (NULL == parent)
        {
            CHECK_OCI
            (
                obj->con->err,
                OCIObjectGetInd,
                obj->con->env->handle, obj->con->err,
                (dvoid *)obj->handle,
                (dvoid **) &obj->tab_ind
            );
        }
        else
        {
            obj->tab_ind = parent->tab_ind;
            obj->idx_ind = parent->idx_ind + OcilibObjectGetIndOffset(parent->typinf, index);
        }
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibObjectFree(obj);
            obj = NULL;
        }

        SET_RETVAL(obj);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetAttributeIndex
 * --------------------------------------------------------------------------------------------- */

static OCI_Int32 OcilibObjectGetAttributeIndex
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Int32    type,
    OCI_Boolean  check
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, -1,
        /* context */ OCI_IPC_OBJECT, obj
    );

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_STRING, attr);

    OCI_Int32 index = -1;

    for (ub2 i = 0; i < obj->typinf->nb_cols; i++)
    {
        OCI_Column *col = &obj->typinf->cols[i];

        if (((type == -1) || (col->datatype == (OCI_SqlType) type))  && (OcilibStringCaseCompare(col->name, attr) == 0))
        {
            index = (OCI_Int32) i;
            break;
        }
    }

    if (check && index == -1)
    {
        THROW(OcilibExceptionAttributeNotFound, attr);
    }

    SET_RETVAL(index);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetAttr
 * --------------------------------------------------------------------------------------------- */

static void * OcilibObjectGetAttr
(
    OCI_Object *obj,
    OCI_Int32   index,
    OCIInd    **pind
)
{
    ENTER_FUNC
    (
        /* returns */ void*, NULL,
        /* context */ OCI_IPC_OBJECT, obj
    );

    OCI_Size offset = 0;
    OCI_Size size   = 0;
    OCI_Size align  = 0;

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_VOID,   pind);

    if (obj->typinf->struct_size == 0)
    {
        OcilibObjectGetStructSize(obj->typinf, &size, &align);
    }

    offset = (OCI_Size) obj->typinf->offsets[index];

    if (pind)
    {
        const OCI_Int32 ind_index = obj->idx_ind + OcilibObjectGetIndOffset(obj->typinf, index);

        *pind = &obj->tab_ind[ind_index];
    }

    SET_RETVAL(((char *) obj->handle + offset));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetNumberInternal
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibObjectSetNumberInternal
(
    OCI_Object  *obj,
    const otext *attr,
    void        *value,
    uword        flag
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );

    OCI_Int32 index = 0;

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_STRING, attr);

    index = OcilibObjectGetAttributeIndex(obj, attr, OCI_CDT_NUMERIC, TRUE);

    if (index == -1)
    {
        THROW(OcilibExceptionAttributeNotFound, attr);
    }

    OCIInd     *ind = NULL;
    void       *num = OcilibObjectGetAttr(obj, index, &ind);
    OCI_Column *col = &obj->typinf->cols[index];

    CHECK(OcilibNumberTranslateValue(obj->con->env, value, flag, num, col->subtype));

    *ind = OCI_IND_NOTNULL;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetNumberInternal
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibObjectGetNumberInternal
(
    OCI_Object  *obj,
    const otext *attr,
    void        *value,
    uword        flag
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );

    OCI_Int32 index = 0;

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_STRING, attr);

    index = OcilibObjectGetAttributeIndex(obj, attr, OCI_CDT_NUMERIC, FALSE);

    if (index >= 0)
    {
        OCIInd *ind = NULL;
        void   *ptr = NULL;

        ptr = OcilibObjectGetAttr(obj, index, &ind);

        if (ptr && (OCI_IND_NULL != *ind))
        {
            OCI_Column *col = &obj->typinf->cols[index];

            CHECK(OcilibNumberTranslateValue(obj->con->env, ptr, col->subtype, value, flag));
        }
    }
    else
    {
        index = OcilibObjectGetAttributeIndex(obj, attr, OCI_CDT_TEXT, FALSE);

        if (index >= 0)
        {
            CHECK(OcilibNumberFromStringInternal(obj->con->env, value, flag,
                                                 OcilibObjectGetString(obj, attr), NULL));
        }
    }

    if (index == -1)
    {
        THROW(OcilibExceptionAttributeNotFound, attr);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OcilibObjectCreate
(
    OCI_TypeInfo *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Object*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    CHECK_PTR(OCI_IPC_TYPE_INFO,  typinf);

    SET_RETVAL(OcilibObjectInitialize(typinf->con, NULL, NULL, typinf, NULL, -1, TRUE));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectFree
(
    OCI_Object *obj
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_OBJECT_FETCHED(obj);

    /* if the object has sub-objects that have been fetched, we need to free
       these objects */

    OcilibObjectReset(obj);

    FREE(obj->objs);
    FREE(obj->tmpbufs);
    FREE(obj->tmpsizes);

    if ((OCI_OBJECT_ALLOCATED == obj->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == obj->hstate))
    {
        OcilibMemoryFreeObject(obj->con->env, obj->con->env->handle, obj->con->err, obj->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != obj->hstate)
    {
        OcilibErrorResetSource(NULL, obj);

        FREE(obj);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Object ** OcilibObjectCreateArray
(
    OCI_TypeInfo *typinf,
    OCI_UInt32    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Object**, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_TYPE_INFO,  typinf);

    arr = OcilibArrayCreate(typinf->con->env,typinf->con, nbelem, OCI_CDT_OBJECT, 0,
                            sizeof(void *), sizeof(OCI_Object),
                            0, typinf);

    CHECK_NULL(arr);

    SET_RETVAL((OCI_Object **) arr->tab_obj);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectFreeArray
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectFreeArray
(
    OCI_Object **objs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, NULL
    );

    CHECK_PTR(OCI_IPC_ARRAY, objs);

    SET_RETVAL(OcilibArrayFreeFromHandles(((OCI_Object *) objs[0])->con->env, (void **)objs));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectAssign
(
    OCI_Object *obj,
    OCI_Object *obj_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_OBJECT, obj_src);
    CHECK_COMPAT(obj->typinf->handle == obj_src->typinf->handle);

    CHECK_OCI
    (
        obj->con->err,
        OCIObjectCopy,
        obj->con->env->handle, obj->con->err, obj->con->handle,
        obj_src->handle, (obj_src->tab_ind + obj_src->idx_ind),
        obj->handle, (obj->tab_ind + obj->idx_ind),
        obj->typinf->handle, OCI_DURATION_SESSION, OCI_DEFAULT
    );

    obj->typinf = obj_src->typinf;

    OcilibObjectReset(obj);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetInt16
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectGetBoolean
(
    OCI_Object  *obj,
    const otext *attr
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );
    OCI_Int32 index = -1;

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_STRING, attr);

    index = OcilibObjectGetAttributeIndex(obj, attr, OCI_CDT_BOOLEAN, TRUE);

    if (index < 0)
    {
        THROW(OcilibExceptionAttributeNotFound, attr);
    }

    OCIInd *ind = NULL;
    void   *ptr = NULL;

    OCI_Boolean value = FALSE;

    ptr = OcilibObjectGetAttr(obj, index, &ind);

    if (NULL != ptr && ind && (OCI_IND_NULL != *ind))
    {
        value = *((OCI_Boolean *) ptr);
    }

    SET_RETVAL(value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetNumber
 * --------------------------------------------------------------------------------------------- */

OCI_Number * OcilibObjectGetNumber
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
        OcilibNumberInitialize,
        obj->con->env,
        (OCI_Number *) obj->objs[index],
        (OCINumber *) value
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetInt16
 * --------------------------------------------------------------------------------------------- */

OCI_Int16 OcilibObjectGetInt16
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_NUMBER(OCI_NUM_INT16, OCI_Int16, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetUInt16
 * --------------------------------------------------------------------------------------------- */

OCI_UInt16 OcilibObjectGetUInt16
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_NUMBER(OCI_NUM_UINT16, OCI_UInt16, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetInt32
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibObjectGetInt32
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_NUMBER(OCI_NUM_INT32, OCI_Int32, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetUInt32
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibObjectGetUInt32
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_NUMBER(OCI_NUM_UINT32, OCI_UInt32, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetInt64
 * --------------------------------------------------------------------------------------------- */

OCI_Int64 OcilibObjectGetInt64
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_NUMBER(OCI_NUM_INT64, OCI_Int64, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetUInt64
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibObjectGetUInt64
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_NUMBER(OCI_NUM_UINT64, OCI_UInt64, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetDouble
 * --------------------------------------------------------------------------------------------- */

OCI_Double OcilibObjectGetDouble
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_NUMBER(OCI_NUM_DOUBLE, OCI_Double, 0.0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetFloat
 * --------------------------------------------------------------------------------------------- */

OCI_Float OcilibObjectGetFloat
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_NUMBER(OCI_NUM_FLOAT, OCI_Float, 0.0f);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibObjectGetString
(
    OCI_Object  *obj,
    const otext *attr
)
{
    ENTER_FUNC
    (
        /* returns */ const otext *, NULL,
        /* context */ OCI_IPC_OBJECT, obj
    );

    OCI_Int32 index = -1;

    const otext* str = NULL;

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_STRING, attr);

    index = OcilibObjectGetAttributeIndex(obj, attr, OCI_CDT_TEXT, FALSE);

    if (index >= 0)
    {
        OCIInd     *ind   = NULL;
        OCIString **value = NULL;

        value = (OCIString **)OcilibObjectGetAttr(obj, index, &ind);

        if (value && ind && (OCI_IND_NULL != *ind))
        {
            oratext *ocistr;

            OCI_CALL(ocistr, OCIStringPtr, obj->con->env->handle, *value);
            str = (otext *) ocistr;
        }
    }
    else
    {
        index = OcilibObjectGetAttributeIndex(obj, attr, -1, FALSE);

        if (index >= 0)
        {
            OCI_Error *err   = OcilibErrorGet(ocilib_call_context.env, TRUE, TRUE);
            OCIInd    *ind   = NULL;
            void      *value = NULL;
            OCI_UInt32 size  = 0;
            OCI_UInt32 len   = 0;

            value = OcilibObjectGetAttr(obj, index, &ind);

            /* special case of RAW attribute, we need their length */
            if (OCI_CDT_RAW == obj->typinf->cols[index].datatype)
            {
                if (value && ind && (OCI_IND_NULL != *ind))
                {
                    OCI_CALL(size, OCIRawSize, obj->con->env->handle, (*(OCIRaw **) value));
                    OCI_CALL(value, OCIRawPtr, obj->con->env->handle,  (*(OCIRaw **) value));
                }
            }

            len = OcilibStringGetFromType(obj->con, &obj->typinf->cols[index], value,
                                          size, NULL, 0, FALSE);

            CHECK_ERROR(err);

            if (len > 0)
            {
                CHECK(OcilibStringRequestBuffer(obj->con->env, &obj->tmpbufs[index], &obj->tmpsizes[index], len));

                const OCI_UInt32 real_tmpsize = OcilibStringGetFromType
                                                (
                                                    obj->con,
                                                    &obj->typinf->cols[index],
                                                    value, size,
                                                    obj->tmpbufs[index],
                                                    obj->tmpsizes[index],
                                                    FALSE
                                                );

                CHECK(NULL == err || OCI_UNKNOWN == err->type);

                if (real_tmpsize > 0)
                {
                    str = obj->tmpbufs[index];
                }
            }
        }
    }

    if (index == -1)
    {
        THROW(OcilibExceptionAttributeNotFound, attr);
    }

    SET_RETVAL(str);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetRaw
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibObjectGetRaw
(
    OCI_Object  *obj,
    const otext *attr,
    void        *buffer,
    OCI_UInt32   len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, 0,
        /* context */ OCI_IPC_OBJECT, obj
    );

    OCI_Int32 index = -1;
    OCI_Int32 size  = 0;

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_STRING, attr);

    index = OcilibObjectGetAttributeIndex(obj, attr, OCI_CDT_RAW, TRUE);

    if (index < 0)
    {
        THROW(OcilibExceptionAttributeNotFound, attr);
    }

    OCIInd  *ind   = NULL;
    OCIRaw **value = NULL;

    value = (OCIRaw **)OcilibObjectGetAttr(obj, index, &ind);

    if (value && ind && (OCI_IND_NULL != *ind))
    {
        ub4 raw_len = 0;

        OCI_CALL(raw_len, OCIRawSize, obj->con->env->handle, *value);

        if (len > raw_len)
        {
            len = raw_len;
        }

        oratext *rawptr = NULL;

        OCI_CALL(rawptr, OCIRawPtr, obj->con->env->handle, *value);

        CHECK_NULL(rawptr);

        memcpy(buffer, rawptr, (OCI_Size) len);

        size = (OCI_Int32) len;
    }

    SET_RETVAL(size);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibObjectGetRawSize
* --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibObjectGetRawSize
(
    OCI_Object  *obj,
    const otext *attr
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_OBJECT, obj
    );

    ub4       raw_len = 0;
    OCI_Int32 index   = -1;

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_STRING, attr);

    index = OcilibObjectGetAttributeIndex(obj, attr, OCI_CDT_RAW, TRUE);

    if (index < 0)
    {
        THROW(OcilibExceptionAttributeNotFound, attr);
    }

    OCIInd  *ind   = NULL;
    OCIRaw **value = NULL;

    value = (OCIRaw **)OcilibObjectGetAttr(obj, index, &ind);

    if (value && ind && (OCI_IND_NULL != *ind))
    {
        OCI_CALL(raw_len, OCIRawSize, obj->con->env->handle, *value);
    }

    SET_RETVAL(raw_len);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OcilibObjectGetDate
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
        OcilibDateInitialize,
        obj->con->env,
        (OCI_Date *) obj->objs[index],
        value,
        FALSE,
        FALSE
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OcilibObjectGetTimestamp
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
        OcilibTimestampInitialize,
        obj->con->env, obj->con,
        (OCI_Timestamp *) obj->objs[index],
        (OCIDateTime *) *value,
        obj->typinf->cols[index].subtype
    );

#else

    ENTER_FUNC
    (
        /* returns */ OCI_Timestamp*, NULL,
        /* context */ OCI_IPC_OBJECT, obj
    );

    CHECK_PTR(OCI_IPC_OBJECT, obj);

    EXIT_FUNC();

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OcilibObjectGetInterval
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
        OcilibIntervalInitialize,
        obj->con->env, obj->con,
        (OCI_Interval *) obj->objs[index],
        (OCIInterval *) *value,
        obj->typinf->cols[index].subtype
    );

#else

    ENTER_FUNC
    (
        /* returns */ OCI_Interval*, NULL,
        /* context */ OCI_IPC_OBJECT, obj
    );

    CHECK_PTR(OCI_IPC_OBJECT, obj);

    EXIT_FUNC();

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetColl
 * --------------------------------------------------------------------------------------------- */

OCI_Collection * OcilibObjectGetColl
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCI_Collection*,
        OCIColl*,
        OcilibCollectionInitialize,
        obj->con,
        (OCI_Collection *) obj->objs[index],
        (OCIColl *) *value,
        obj->typinf->cols[index].typinf
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OcilibObjectGetObject
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
        OcilibObjectInitialize,
        obj->con,
        (OCI_Object *) obj->objs[index],
        value,
        obj->typinf->cols[index].typinf,
        obj, index, FALSE
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OcilibObjectGetLob
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
        OcilibLobInitialize,
        obj->con,
        (OCI_Lob *) obj->objs[index],
        *value,
        obj->typinf->cols[index].subtype
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * OcilibObjectGetFile
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
        OcilibFileInitialize,
        obj->con,
        (OCI_File *) obj->objs[index],
        *value,
        obj->typinf->cols[index].subtype
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetReference
 * --------------------------------------------------------------------------------------------- */

OCI_Reference * OcilibObjectGetReference
(
    OCI_Object  *obj,
    const otext *attr
)
{
    OBJECT_GET_VALUE
    (
        OCI_CDT_REF,
        OCI_Reference*,
        OCIRef*,
        OcilibReferenceInitialize,
        obj->con,
        NULL,
        (OCI_Reference *) obj->objs[index],
        *value
    );
}

/* --------------------------------------------------------------------------------------------- *
* OcilibObjectSetBoolean
* --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetBoolean
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Boolean  value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_STRING, attr);

    const OCI_Int32 index = OcilibObjectGetAttributeIndex(obj, attr, OCI_CDT_BOOLEAN, TRUE);

    if (index < 0)
    {
        THROW(OcilibExceptionAttributeNotFound, attr);
    }

    OCIInd      *ind  = NULL;
    OCI_Boolean *data = (OCI_Boolean *)OcilibObjectGetAttr(obj, index, &ind);

    if (data)
    {
        *data = value;
        *ind  = OCI_IND_NOTNULL;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetNumber
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetNumber
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
        OCINumberAssign,
        obj->con->err,
        value->handle,
        data
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetInt16
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetInt16
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Int16    value
)
{
    OBJECT_SET_NUMBER(OCI_NUM_INT16, value);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetUInt16
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetUInt16
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_UInt16   value
)
{
    OBJECT_SET_NUMBER(OCI_NUM_UINT16, value);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetInt32
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetInt32
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Int32    value
)
{
    OBJECT_SET_NUMBER(OCI_NUM_INT32, value);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetUInt32
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetUInt32
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_UInt32   value
)
{
    OBJECT_SET_NUMBER(OCI_NUM_UINT32, value);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetInt64
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetInt64
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Int64    value
)
{
    OBJECT_SET_NUMBER(OCI_NUM_INT64, value);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetUInt64
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetUInt64
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_UInt64   value
)
{
    OBJECT_SET_NUMBER(OCI_NUM_UINT64, value);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetDouble
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetDouble
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Double   value
)
{
    OBJECT_SET_NUMBER(OCI_NUM_DOUBLE, value);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetFloat
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetFloat
(
    OCI_Object  *obj,
    const otext *attr,
    OCI_Float    value
)
{
    OBJECT_SET_NUMBER(OCI_NUM_FLOAT, value);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetString
(
    OCI_Object  *obj,
    const otext *attr,
    const otext *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );
    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_STRING, attr);

    if (NULL == value)
    {
        CHECK(OcilibObjectSetNull(obj, attr));
    }
    else
    {
        const OCI_Int32 index = OcilibObjectGetAttributeIndex(obj, attr, OCI_CDT_TEXT, TRUE);

        if (index < 0)
        {
            THROW(OcilibExceptionAttributeNotFound, attr);
        }

        OCIInd     *ind  = NULL;
        OCIString **data = (OCIString **)OcilibObjectGetAttr(obj, index, &ind);

        CHECK(OcilibStringToStringPtr(obj->con->env, obj->con->env->handle, data, obj->con->err, value));

        *ind = OCI_IND_NOTNULL;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetRaw
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetRaw
(
    OCI_Object  *obj,
    const otext *attr,
    void       * value,
    OCI_UInt32   len
)
{
    OBJECT_SET_VALUE
    (
        OCI_CDT_RAW,
        OCIRaw*,
        OCIRawAssignBytes,
        obj->con->env->handle,
        obj->con->err,
        (ub1*) value,
        len,
        data
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetDate
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
        OCIDateAssign,
        obj->con->err,
        value->handle,
        data
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetTimestamp
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
        OCIDateTimeAssign,
        (dvoid *) obj->con->env->handle,
        obj->con->err,
        value->handle, *data
    );

#else

    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );

    CHECK_PTR(OCI_IPC_OBJECT, obj);

    EXIT_FUNC();

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetInterval
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
        OCIIntervalAssign,
        (dvoid *) obj->con->env->handle,
        obj->con->err,
        value->handle,
        *data
    );

#else

    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );

    CHECK_PTR(OCI_IPC_OBJECT, obj);

    EXIT_FUNC();

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetColl
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetColl
(
    OCI_Object     *obj,
    const otext    *attr,
    OCI_Collection *value
)
{
    OBJECT_SET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCIColl*,
        OCICollAssign,
        obj->con->env->handle,
        obj->con->err,
        value->handle,
        *data
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetObject
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
        OCIObjectCopy,
        obj->con->env->handle,
        obj->con->err,
        obj->con->handle,
        value->handle,
        (value->tab_ind + value->idx_ind),
        data,
        ind,
        obj->typinf->cols[index].typinf->handle,
        OCI_DURATION_SESSION,
        OCI_DEFAULT
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetLob
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
        OCILobLocatorAssign,
        obj->con->handle,
        obj->con->err,
        value->handle,
        (OCILobLocator **) data
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetFile
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetFile
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
        OCILobLocatorAssign,
        obj->con->handle,
        obj->con->err,
        value->handle,
        (OCILobLocator **) data
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetReference
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetReference
(
    OCI_Object    *obj,
    const otext   *attr,
    OCI_Reference *value
)
{
    OBJECT_SET_VALUE
    (
        OCI_CDT_REF,
        OCIRef*,
        OCIRefAssign,
        obj->con->env->handle,
        obj->con->err,
        value->handle,
        data
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectSetNull
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectSetNull
(
    OCI_Object  *obj,
    const otext *attr
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );
    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_STRING, attr);

    const OCI_Int32 index = OcilibObjectGetAttributeIndex(obj, attr, -1, TRUE);

    if (index == -1)
    {
        THROW(OcilibExceptionAttributeNotFound, attr);
    }

    const OCI_Int32 ind_index = obj->idx_ind + OcilibObjectGetIndOffset(obj->typinf, index);

    obj->tab_ind[ind_index] = OCI_IND_NULL;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectIsNull
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectIsNull
(
    OCI_Object  *obj,
    const otext *attr
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );

    OCI_Int32 index = 0;

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_STRING, attr);

    index = OcilibObjectGetAttributeIndex(obj, attr, -1, TRUE);

    if (index == -1)
    {
        THROW(OcilibExceptionAttributeNotFound, attr);
    }

    const OCI_Int32 ind_index = obj->idx_ind + OcilibObjectGetIndOffset(obj->typinf, index);

    SET_RETVAL(OCI_IND_NOTNULL != obj->tab_ind[ind_index]);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OcilibObjectGetTypeInfo
(
    OCI_Object *obj
)
{
    GET_PROP
    (
        OCI_TypeInfo*, NULL,
        OCI_IPC_OBJECT, obj,
        typinf
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetType
 * --------------------------------------------------------------------------------------------- */

OCI_ObjectType OcilibObjectGetType
(
    OCI_Object *obj
)
{
    GET_PROP
    (
        OCI_ObjectType, OCI_UNKNOWN,
        OCI_IPC_OBJECT, obj,
        type
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetSelfReference
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectGetSelfReference
(
    OCI_Object    *obj,
    OCI_Reference *ref
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_REF,    ref);
    CHECK_COMPAT(obj->typinf->handle == ref->typinf->handle);

    CHECK_OCI
    (
        obj->con->err,
        OCIObjectGetObjectRef,
        obj->con->env->handle, obj->con->err,
        obj->handle, ref->handle
    );

    if (NULL != ref->obj)
    {
        OcilibObjectFree(ref->obj);
        ref->obj = NULL;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectGetStruct
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectGetStruct
(
    OCI_Object *obj,
    void      **pp_struct,
    void      **pp_ind
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );
    CHECK_PTR(OCI_IPC_OBJECT, obj);

    if (pp_struct)
    {
        *pp_struct = (void *) obj->handle;
    }

    if (pp_ind)
    {
        *pp_ind = (void *) obj->tab_ind;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibObjectToString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibObjectToString
(
    OCI_Object *obj,
    OCI_UInt32 *size,
    otext      *str
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_OBJECT, obj
    );

    OCI_Error  *err   = NULL;
    otext      *attr  = NULL;
    OCI_Boolean quote = TRUE;
    OCI_UInt32  len   = 0;

    CHECK_PTR(OCI_IPC_OBJECT, obj);
    CHECK_PTR(OCI_IPC_VOID,   size);

    err = OcilibErrorGet(ocilib_call_context.env, TRUE, TRUE);

    if (str)
    {
        *str = 0;
    }

    len += OcilibStringAddToBuffer(str, len, obj->typinf->name, (OCI_UInt32) ostrlen(obj->typinf->name), FALSE);
    len += OcilibStringAddToBuffer(str, len, OTEXT("("), 1, FALSE);

    for (OCI_Int32 i = 0; i < obj->typinf->nb_cols; i++)
    {
        attr  = obj->typinf->cols[i].name;
        quote = TRUE;

        if (OcilibObjectIsNull(obj, attr))
        {
            len += OcilibStringAddToBuffer(str, len, OCI_STRING_NULL, OCI_STRING_NULL_SIZE, FALSE);
        }
        else
        {
            void            *data      = NULL;
            OCI_UInt32       data_size = 0;
            const OCI_UInt32 data_type = obj->typinf->cols[i].datatype;

            switch (data_type)
            {
                case OCI_CDT_TEXT:
                {
                    OCIInd *ind = NULL;
                    data = OcilibObjectGetAttr(obj, i, &ind);

                    if (data && ind && (OCI_IND_NULL != *ind))
                    {
                        OCI_CALL(data_size, OCIStringSize, obj->typinf->con->env->handle, (*(OCIString **)data));
                        data = (void *)OcilibObjectGetString(obj, attr);
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
                    data  = OcilibObjectGetAttr(obj, i, &ind);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_NUMERIC:
                {
                    OCIInd *ind = NULL;
                    data  = OcilibObjectGetAttr(obj, i, &ind);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_RAW:
                {
                    OCIInd *ind = NULL;
                    data = OcilibObjectGetAttr(obj, i, &ind);

                    if (data && ind && (OCI_IND_NULL != *ind))
                    {
                        OCI_CALL(data_size, OCIRawSize, obj->con->env->handle, (*(OCIRaw **) data));
                        OCI_CALL(data, OCIRawPtr, obj->con->env->handle,  (*(OCIRaw **) data));
                    }
                    else
                    {
                        data = NULL;
                    }
                    break;
                }
                case OCI_CDT_DATETIME:
                {
                    data = (void *)OcilibObjectGetDate(obj, attr);
                    break;
                }
                case OCI_CDT_TIMESTAMP:
                {
                    data = (void *)OcilibObjectGetTimestamp(obj, attr);
                    break;
                }
                case OCI_CDT_INTERVAL:
                {
                    data = (void *)OcilibObjectGetInterval(obj, attr);
                    break;
                }
                case OCI_CDT_LOB:
                {
                    data = (void *)OcilibObjectGetLob(obj, attr);
                    break;
                }
                case OCI_CDT_FILE:
                {
                    data = (void *)OcilibObjectGetFile(obj, attr);
                    break;
                }
                case OCI_CDT_REF:
                {
                    data = (void *)OcilibObjectGetReference(obj, attr);
                    break;
                }
                case OCI_CDT_OBJECT:
                {
                    data  = (void *)OcilibObjectGetObject(obj, attr);
                    quote = FALSE;
                    break;
                }
                case OCI_CDT_COLLECTION:
                {
                    data  =  (void *)OcilibObjectGetColl(obj, attr);
                    quote = FALSE;
                }
            }

            CHECK(NULL != data || OCI_CDT_TEXT == data_type);
            CHECK_ERROR(err);

            otext *tmpbuf = str;

            if (tmpbuf)
            {
                tmpbuf += len;
            }

            if (data)
            {
                len += OcilibStringGetFromType(obj->con, &obj->typinf->cols[i], data, data_size,
                                               tmpbuf, tmpbuf && size ? *size - len : 0, quote);
            }
            else
            {
                len += OcilibStringAddToBuffer(str, len, OCI_STRING_NULL, OCI_STRING_NULL_SIZE, FALSE);
            }

            CHECK(NULL == err || OCI_UNKNOWN == err->type);
        }

        if (i < (obj->typinf->nb_cols-1))
        {
            len += OcilibStringAddToBuffer(str, len, OTEXT(", "), 2, quote);
        }
    }

    len += OcilibStringAddToBuffer(str, len, OTEXT(")"), 1, FALSE);

    *size = len;

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            *size = 0;

            if (NULL != str)
            {
                *str = 0;
            }
        }
    );
}
