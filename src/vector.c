/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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

#include "vector.h"

#include "array.h"
#include "macros.h"
#include "memory.h"
#include "stringutils.h"

#if OCI_VERSION_COMPILE >= OCI_23_4

static OCI_UInt32 VectorFormatValues[] =
{
    OCI_VEC_FLOAT16,
    OCI_VEC_FLOAT32,
    OCI_VEC_FLOAT64,
    OCI_VEC_INT8,
    OCI_VEC_BINARY
};

static OCI_UInt32 VectorSparseFormatValues[] =
{
    OCI_VEC_FLOAT32,
    OCI_VEC_FLOAT64,
    OCI_VEC_INT8,
    OCI_VEC_BINARY
};

/* Oracle clients >= 23.7 exchange vector text in the environment charset
   (UTF-16 in wide builds, sized in bytes) instead of always using MBCS.
   Known Oracle client issue: in UTF-16 environments, OCIVectorToText only
   writes as many bytes as there are characters, so its output is truncated */

#define VECTOR_TEXT_USES_ENV_CHARSET(v) \
        (IS_WIDE_ENV() && (v)->con->env->version_runtime >= OCI_23_7)

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorGetFormatSize
 * --------------------------------------------------------------------------------------------- */

static ub4 OcilibVectorGetFormatSize
(
    OCI_UInt32 format
)
{
    switch (format)
    {
        case OCI_VEC_FLOAT16:
            return (ub4) sizeof(OCI_UInt16);
        case OCI_VEC_FLOAT32:
            return (ub4) sizeof(OCI_Float);
        case OCI_VEC_FLOAT64:
            return (ub4) sizeof(OCI_Double);
        case OCI_VEC_INT8:
            return (ub4) sizeof(char);
        case OCI_VEC_BINARY:
            return (ub4) sizeof(unsigned char);
    }

    return 0;
}

#endif

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Vector * OcilibVectorInitialize
(
    OCI_Connection *con,
    OCI_Vector     *vect,
#if OCI_VERSION_COMPILE >= OCI_23_4
    OCIVector      *handle
#else
    void *handle
#endif
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Vector*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    ALLOC_HANDLE(OCI_IPC_VECTOR, vect);
    vect->con    = con;
    vect->handle = handle;

    /* allocate buffer if needed */

    if (NULL == vect->handle || (OCI_OBJECT_ALLOCATED_ARRAY == vect->hstate))
    {
        if (OCI_OBJECT_ALLOCATED_ARRAY != vect->hstate)
        {
            CHECK
            (
                OcilibMemoryAllocDescriptor
                (
                    vect->con->env,
                    (dvoid *)vect->con->env->handle,
                    (dvoid **)(void *)&vect->handle,
                    (ub4)OCI_DTYPE_VECTOR
                )
            );

            vect->hstate = OCI_OBJECT_ALLOCATED;
        }
    }
    else
    {
        vect->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

#else

    OCI_NOT_USED(con);
    OCI_NOT_USED(handle);

    CHECK(FALSE);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE && NULL != vect)
        {
            OcilibVectorFree(vect);
            vect = NULL;
        }

        SET_RETVAL(vect);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Vector * OcilibVectorCreate
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Vector*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_VECTOR_ENABLED(con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    SET_RETVAL(OcilibVectorInitialize(con, NULL, NULL));

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibVectorFree
(
    OCI_Vector *vect
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    );

    CHECK_PTR(OCI_IPC_VECTOR, vect);
    CHECK_VECTOR_ENABLED(vect->con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    CHECK_OBJECT_FETCHED(vect);

    if (OCI_OBJECT_ALLOCATED == vect->hstate)
    {
        OcilibMemoryFreeDescriptor
        (
            vect->con->env,
            (dvoid*)vect->handle,
            OCI_DTYPE_VECTOR
        );
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != vect->hstate)
    {
        OcilibErrorResetSource(NULL, vect);

        FREE(vect);
    }

    SET_SUCCESS();

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Vector ** OcilibVectorCreateArray
(
    OCI_Connection *con,
    OCI_UInt32      nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Vector**, NULL,
        /* context */ (con ? OCI_IPC_CONNECTION : OCI_IPC_VOID), (con ? (void*)con : (void*)NULL)
    );

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_VECTOR_ENABLED(con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    arr = OcilibArrayCreate(con->env,con, nbelem, OCI_CDT_VECTOR, 0,
                            sizeof(OCIVector *), sizeof(OCI_Vector),
                            0, NULL);

    CHECK_NULL(arr);

    SET_RETVAL((OCI_Vector**)arr->tab_obj);

#else

    OCI_NOT_USED(arr);
    OCI_NOT_USED(nbelem);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibbVectorFreeArray
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibVectorFreeArray
(
    OCI_Vector **vects
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, NULL
    );

    CHECK_PTR(OCI_IPC_ARRAY, vects);

    SET_RETVAL(OcilibArrayFreeFromHandles(((OCI_Vector *) vects[0])->con->env, (void **) vects));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorGetInfo
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibVectorGetInfo
(
    OCI_Vector *vect,
    OCI_UInt32 *format,
    OCI_UInt32 *dimensions
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    );

    CHECK_PTR(OCI_IPC_VECTOR, vect);
    CHECK_VECTOR_ENABLED(vect->con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    ub4 oci_dimensions = 0;
    ub1 oci_format     = OCI_VEC_FLEX;

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_VECTOR, OCI_ATTR_VECTOR_DIMENSION,
        vect->handle, &oci_dimensions, NULL,
        vect->con->err
    );

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_VECTOR, OCI_ATTR_VECTOR_DATA_FORMAT,
        vect->handle, &oci_format, NULL,
        vect->con->err
    );

    *dimensions = (OCI_UInt32) oci_dimensions;
    *format     = (OCI_UInt32) oci_format;

    SET_SUCCESS();

#else

    OCI_NOT_USED(format);
    OCI_NOT_USED(dimensions);

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorGetValues
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibVectorGetValues
(
    OCI_Vector *vect,
    void       *values
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    );

    CHECK_PTR(OCI_IPC_VECTOR, vect);
    CHECK_PTR(OCI_IPC_VOID, values);
    CHECK_VECTOR_ENABLED(vect->con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    OCI_UInt32       dimensions = 0;
    OCI_VectorFormat format     = OCI_VEC_FLEX;

    CHECK(OcilibVectorGetInfo(vect, &format, &dimensions));

    ub4 oci_dimensions = (ub4) dimensions;
    ub1 oci_format     = (ub1) format;

    CHECK_OCI
    (
        vect->con->err,
        OCIVectorToArray,
        vect->handle,
        vect->con->err,
        oci_format,
        &oci_dimensions,
        values,
        OCI_DEFAULT
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(values);

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorSetValues
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibVectorSetValues
(
    OCI_Vector      *vect,
    OCI_VectorFormat format,
    OCI_UInt32       dimensions,
    void            *values
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    );

    CHECK_PTR(OCI_IPC_VECTOR, vect);
    CHECK_PTR(OCI_IPC_VOID, values);
    CHECK_VECTOR_ENABLED(vect->con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    CHECK_ENUM_VALUE(format, VectorFormatValues, OTEXT("Vector format"));

    CHECK_OCI
    (
        vect->con->err,
        OCIVectorFromArray,
        vect->handle,
        vect->con->err,
        (ub1) format,
        (ub4) dimensions,
        values,
        OCI_DEFAULT
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(format);
    OCI_NOT_USED(dimensions);
    OCI_NOT_USED(values);

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorGetProperties
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibVectorGetProperties
(
    OCI_Vector *vect,
    OCI_UInt32 *properties
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    );

    CHECK_PTR(OCI_IPC_VECTOR, vect);
    CHECK_PTR(OCI_IPC_VOID, properties);
    CHECK_VECTOR_ENABLED(vect->con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    ub4 oci_properties = 0;

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_VECTOR, OCI_ATTR_VECTOR_PROPERTY,
        vect->handle, &oci_properties, NULL,
        vect->con->err
    );

    *properties = (OCI_UInt32) oci_properties;

    SET_SUCCESS();

#else

    OCI_NOT_USED(properties);

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorIsSparse
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibVectorIsSparse
(
    OCI_Vector *vect
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    );

    CHECK_PTR(OCI_IPC_VECTOR, vect);
    CHECK_VECTOR_ENABLED(vect->con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    OCI_UInt32 properties = 0;

    CHECK(OcilibVectorGetProperties(vect, &properties));

    SET_RETVAL(0 != (properties & OCI_ATTR_VECTOR_COL_PROPERTY_IS_SPARSE));

#else

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorGetSparseValues
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibVectorGetSparseValues
(
    OCI_Vector *vect,
    OCI_UInt32 *indices,
    void       *values,
    OCI_UInt32 *count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    );

    CHECK_PTR(OCI_IPC_VECTOR, vect);
    CHECK_PTR(OCI_IPC_VOID, indices);
    CHECK_PTR(OCI_IPC_VOID, values);
    CHECK_PTR(OCI_IPC_VOID, count);
    CHECK_SPARSE_VECTOR_ENABLED(vect->con);

#if OCI_VERSION_COMPILE >= OCI_23_7

    OCI_UInt32 format     = OCI_VEC_FLEX;
    OCI_UInt32 dimensions = 0;
    ub4        vdim       = (ub4) *count;
    ub4        actual     = (ub4) *count;

    CHECK(OcilibVectorGetInfo(vect, &format, &dimensions));

    CHECK_OCI
    (
        vect->con->err,
        OCIVectorToSparseArray,
        vect->handle,
        vect->con->err,
        (ub1) format,
        &vdim,
        &actual,
        indices,
        values,
        OCI_DEFAULT
    );

    *count = (OCI_UInt32) actual;

    SET_SUCCESS();

#else

    OCI_NOT_USED(indices);
    OCI_NOT_USED(values);
    OCI_NOT_USED(count);

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorSetSparseValues
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibVectorSetSparseValues
(
    OCI_Vector      *vect,
    OCI_VectorFormat format,
    OCI_UInt32       dimensions,
    OCI_UInt32       count,
    OCI_UInt32      *indices,
    void            *values
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    );

    CHECK_PTR(OCI_IPC_VECTOR, vect);
    CHECK_PTR(OCI_IPC_VOID, indices);
    CHECK_PTR(OCI_IPC_VOID, values);
    CHECK_SPARSE_VECTOR_ENABLED(vect->con);

#if OCI_VERSION_COMPILE >= OCI_23_7

    CHECK_ENUM_VALUE(format, VectorSparseFormatValues, OTEXT("Vector format"));

    CHECK_OCI
    (
        vect->con->err,
        OCIVectorFromSparseArray,
        vect->handle,
        vect->con->err,
        (ub1) format,
        (ub4) dimensions,
        (ub4) count,
        indices,
        values,
        OCI_DEFAULT
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(format);
    OCI_NOT_USED(dimensions);
    OCI_NOT_USED(count);
    OCI_NOT_USED(indices);
    OCI_NOT_USED(values);

    CHECK(FALSE);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorFromString
 * --------------------------------------------------------------------------------------------- */

OCI_SYM_LOCAL OCI_Boolean OcilibVectorFromString
(
    OCI_Vector      *vect,
    const otext    * str,
    OCI_UInt32       size,
    OCI_VectorFormat format,
    OCI_UInt32       dimensions
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    );

    char *mbcs_str = NULL;

    CHECK_PTR(OCI_IPC_VECTOR, vect);
    CHECK_PTR(OCI_IPC_VOID, str);
    CHECK_VECTOR_ENABLED(vect->con);

    ub4 str_size = (ub4) (size == 0 ? ostrlen(str)  : size);

#if OCI_VERSION_COMPILE >= OCI_23_4

    const OraText *oci_str  = (const OraText *) str;
    ub4            oci_size = str_size;

    CHECK_ENUM_VALUE(format, VectorFormatValues, OTEXT("Vector format"));

    if (VECTOR_TEXT_USES_ENV_CHARSET(vect))
    {
        oci_size = (ub4) (str_size * sizeof(dbtext));
    }
    else if (IS_WIDE_ENV())
    {
        mbcs_str = OcilibMemoryAlloc(vect->con->env,OCI_IPC_STRING, (OCI_Size)str_size+1, (OCI_Size)1, TRUE);
        CHECK_NULL(mbcs_str);

        OcilibStringNativeToMbcs(str, mbcs_str, (OCI_Size) str_size);

        oci_str = (const OraText *) mbcs_str;
    }

    CHECK_OCI
    (
        vect->con->err,
        OCIVectorFromText,
        vect->handle,
        vect->con->err,
        (ub1) format,
        (ub4) dimensions,
        oci_str,
        oci_size,
        OCI_DEFAULT
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(str);
    OCI_NOT_USED(str_size);
    OCI_NOT_USED(format);
    OCI_NOT_USED(dimensions);

    CHECK(FALSE);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != mbcs_str)
        {
            OcilibMemoryFree(vect->con->env, mbcs_str);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorToString
 * --------------------------------------------------------------------------------------------- */

OCI_SYM_LOCAL OCI_Boolean OcilibVectorToString
(
    OCI_Vector * vect,
    OCI_UInt32 * size,
    otext      * str

)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    );

    char *mbcs_str = NULL;

    CHECK_PTR(OCI_IPC_VECTOR, vect);
    CHECK_PTR(OCI_IPC_VOID, size);
    CHECK_VECTOR_ENABLED(vect->con);

    ub4 mbcs_size = (ub4) *size;

#if OCI_VERSION_COMPILE >= OCI_23_4

    if (str == NULL)
    {
        size_t max_size = 0;

        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_VECTOR, OCI_ATTR_VECTOR_MAX_STRING_SIZE,
            vect->handle, &max_size, NULL,
            vect->con->err
        );

        mbcs_size = (ub4) max_size;
    }
    else
    {
        str[0] = 0;

        if (VECTOR_TEXT_USES_ENV_CHARSET(vect))
        {
            /* the returned size is the number of bytes actually written */

            mbcs_size = (ub4) (mbcs_size * sizeof(dbtext));

            CHECK_OCI
            (
                vect->con->err,
                OCIVectorToText,
                vect->handle,
                vect->con->err,
                (OraText*)str,
                &mbcs_size,
                OCI_DEFAULT
            );

            mbcs_size = (ub4) (mbcs_size / sizeof(dbtext));

            str[mbcs_size] = 0;
        }
        else
        {
            if (IS_WIDE_ENV())
            {
                mbcs_str = OcilibMemoryAlloc(vect->con->env,OCI_IPC_STRING, (OCI_Size)mbcs_size+1, (OCI_Size)1, TRUE);
                CHECK_NULL(mbcs_str);
            }
            else
            {
                mbcs_str = (char*) str;
            }

            CHECK_OCI
            (
                vect->con->err,
                OCIVectorToText,
                vect->handle,
                vect->con->err,
                (OraText*)mbcs_str,
                &mbcs_size,
                OCI_DEFAULT
            );

            if (IS_WIDE_ENV())
            {
                OcilibStringMbcsToNative(mbcs_str, str, (OCI_Size) mbcs_size);
            }
            else
            {
                str[mbcs_size] = 0;
            }
        }
    }

    *size = (OCI_UInt32)mbcs_size;

#else

    OCI_NOT_USED(size);
    OCI_NOT_USED(str);

    CHECK(FALSE);

#endif

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (IS_WIDE_ENV())
        {
            OcilibMemoryFree(vect->con->env, mbcs_str);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibVectorAssign
(
    OCI_Vector *vect,
    OCI_Vector *vect_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    );

    otext *str    = NULL;
    ub4   *indarr = NULL;
    void  *buffer = NULL;

    CHECK_PTR(OCI_IPC_VECTOR, vect);
    CHECK_PTR(OCI_IPC_VECTOR, vect_src);
    CHECK_VECTOR_ENABLED(vect->con);

#if OCI_VERSION_COMPILE >= OCI_23_4

    OCI_UInt32       dimensions = 0;
    OCI_UInt32       properties = 0;
    OCI_VectorFormat format     = OCI_VEC_FLEX;
    boolean          use_text   = TRUE;
    boolean          sparse     = FALSE;

    CHECK(OcilibVectorGetInfo(vect_src, &format, &dimensions));
    CHECK(OcilibVectorGetProperties(vect_src, &properties));

    sparse = (0 != (properties & OCI_ATTR_VECTOR_COL_PROPERTY_IS_SPARSE));

    if (sparse)
    {
        /* preserve the sparse storage representation when the 23.7
           conversion functions are available; otherwise fall back to
           the text round-trip which also keeps the sparse form        */

        if (NULL != vect->con->env->api->OCIVectorToSparseArray &&
            NULL != vect->con->env->api->OCIVectorFromSparseArray)
        {
            ub4 elem_size = OcilibVectorGetFormatSize(format);
            ub4 vdim      = (ub4) dimensions;
            ub4 actual    = (ub4) dimensions;

            indarr = (ub4*) OcilibMemoryAlloc(vect->con->env, OCI_IPC_BUFF_ARRAY,
                                              (OCI_Size) sizeof(ub4), (OCI_Size) dimensions, TRUE);
            CHECK_NULL(indarr);

            buffer = OcilibMemoryAlloc(vect->con->env, OCI_IPC_BUFF_ARRAY,
                                       (OCI_Size) elem_size, (OCI_Size) dimensions, TRUE);
            CHECK_NULL(buffer);

            CHECK_OCI
            (
                vect_src->con->err,
                OCIVectorToSparseArray,
                vect_src->handle,
                vect_src->con->err,
                (ub1) format,
                &vdim,
                &actual,
                indarr,
                buffer,
                OCI_DEFAULT
            );

            CHECK_OCI
            (
                vect->con->err,
                OCIVectorFromSparseArray,
                vect->handle,
                vect->con->err,
                (ub1) format,
                (ub4) dimensions,
                actual,
                indarr,
                buffer,
                OCI_DEFAULT
            );

            use_text = FALSE;
        }
    }
    else if (OCI_VEC_FLEX != format && dimensions > 0)
    {
        /* dense array copy — lossless and charset-free */

        ub4 elem_size = OcilibVectorGetFormatSize(format);

        buffer = OcilibMemoryAlloc(vect->con->env, OCI_IPC_BUFF_ARRAY,
                                   (OCI_Size) elem_size, (OCI_Size) dimensions, TRUE);
        CHECK_NULL(buffer);

        CHECK(OcilibVectorGetValues(vect_src, buffer));
        CHECK(OcilibVectorSetValues(vect, format, dimensions, buffer));

        use_text = FALSE;
    }

    if (use_text)
    {
        OCI_UInt32 size = 0;

        CHECK(OcilibVectorToString(vect_src, &size, NULL));

        str = (otext *) OcilibMemoryAlloc(vect->con->env, OCI_IPC_STRING,
                                          sizeof(otext), (OCI_Size) size + 1, TRUE);
        CHECK_NULL(str);

        CHECK(OcilibVectorToString(vect_src, &size, str));
        CHECK(OcilibVectorFromString(vect, str, size, format, dimensions));
    }

    SET_SUCCESS();

#else

    OCI_NOT_USED(vect_src);

    CHECK(FALSE);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != str)
        {
            OcilibMemoryFree(vect->con->env, str);
        }

        if (NULL != indarr)
        {
            OcilibMemoryFree(vect->con->env, indarr);
        }

        if (NULL != buffer)
        {
            OcilibMemoryFree(vect->con->env, buffer);
        }
    );
}
