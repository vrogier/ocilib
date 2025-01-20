/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2025 Vincent ROGIER <vince.rogier@ocilib.net>
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

#if OCI_VERSION_COMPILE >= OCI_9_0

static unsigned int VectorFormatValues[] =
{
    OCI_VEC_FLOAT32,
    OCI_VEC_FLOAT64,
    OCI_VEC_INT8,
    OCI_VEC_BINARY
};

#endif
/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Vector * OcilibVectorInitialize
(
    OCI_Connection *con,
    OCI_Vector     *vect,
    OCIVector      *handle
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Vector*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

#if OCI_VERSION_COMPILE >= OCI_23_4

    ALLOC_DATA(OCI_IPC_VECTOR, vect, 1);

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
                    (dvoid  *)vect->con->env,
                    (dvoid **)(void *)&vect->handle,
                    (ub4)OCI_DTYPE_VECTOR
                )
            )

            vect->hstate = OCI_OBJECT_ALLOCATED;
        }
    }
    else
    {
        vect->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

#else

    OCI_NOT_USED(con)
    OCI_NOT_USED(handle)

    CHECK(FALSE)

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibVectorFree(vect);
            vect = NULL;
        }

        SET_RETVAL(vect)
    )
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
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_VECTOR_ENABLED(con)

#if OCI_VERSION_COMPILE >= OCI_23_4

    SET_RETVAL(OcilibVectorInitialize(con, NULL, NULL))

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibVectorFree
(
    OCI_Vector *vect
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    )

    CHECK_PTR(OCI_IPC_VECTOR, vect)
    CHECK_VECTOR_ENABLED(vect->con)

#if OCI_VERSION_COMPILE >= OCI_23_4

   CHECK_OBJECT_FETCHED(vect)

    if (OCI_OBJECT_ALLOCATED == vect->hstate)
    {
        OcilibMemoryFreeDescriptor
        (
            (dvoid*)vect->handle,
            OCI_DTYPE_VECTOR
        );
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != vect->hstate)
    {
        OcilibErrorResetSource(NULL, vect);

        FREE(vect)
    }

    SET_SUCCESS()

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Vector ** OcilibVectorCreateArray
(
    OCI_Connection *con,
    unsigned int    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Vector**, NULL,
        /* context */ (con ? OCI_IPC_CONNECTION : OCI_IPC_VOID), (con ? (void*)con : (void*)&Env)
    )

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_VECTOR_ENABLED(con)

#if OCI_VERSION_COMPILE >= OCI_23_4

     arr = OcilibArrayCreate(con, nbelem, OCI_CDT_VECTOR, 0,
                            sizeof(OCIVector *), sizeof(OCI_Vector),
                            0, NULL);

    CHECK_NULL(arr)

    SET_RETVAL((OCI_Vector**)arr->tab_obj)

#else

    OCI_NOT_USED(arr)
    OCI_NOT_USED(nbelem)

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibbVectorFreeArray
 * --------------------------------------------------------------------------------------------- */


boolean OcilibVectorFreeArray
(
    OCI_Vector **vects
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_PTR(OCI_IPC_ARRAY, vects)

    SET_RETVAL(OcilibArrayFreeFromHandles((void **) vects))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorGetInfo
 * --------------------------------------------------------------------------------------------- */

boolean OcilibVectorGetInfo
(
    OCI_Vector   *vect,
    unsigned int *format,
    unsigned int *dimensions
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    )

    CHECK_PTR(OCI_IPC_VECTOR, vect)
    CHECK_VECTOR_ENABLED(vect->con)

#if OCI_VERSION_COMPILE >= OCI_23_4

    ub4 oci_dimensions = 0;
    ub1 oci_format = OCI_VEC_FLEX;

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_VECTOR, OCI_ATTR_VECTOR_DIMENSION,
        vect->handle, &oci_dimensions, NULL,
        vect->con->err
    )

    CHECK_ATTRIB_GET
    (
        OCI_DTYPE_VECTOR, OCI_ATTR_VECTOR_DATA_FORMAT,
        vect->handle, &oci_format, NULL,
        vect->con->err
    )

    *dimensions  = (unsigned int) oci_dimensions;
    *format      = (unsigned int) oci_format;

    SET_SUCCESS()

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorGetValues
 * --------------------------------------------------------------------------------------------- */

boolean OcilibVectorGetValues
(
    OCI_Vector *vect,
    void       *values
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    )

    CHECK_PTR(OCI_IPC_VECTOR, vect)
    CHECK_PTR(OCI_IPC_VOID, values)
    CHECK_VECTOR_ENABLED(vect->con)

#if OCI_VERSION_COMPILE >= OCI_23_4

    unsigned int dimensions = 0;
    unsigned int format     = OCI_VEC_FLEX;

    CHECK(OcilibVectorGetInfo(vect, &format, &dimensions))

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
    )

    SET_SUCCESS()

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorSetValues
 * --------------------------------------------------------------------------------------------- */

boolean OcilibVectorSetValues
(
    OCI_Vector       *vect,
    unsigned int     format,
    unsigned int     dimensions,
    void            *values
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    )

    CHECK_PTR(OCI_IPC_VECTOR, vect)
    CHECK_PTR(OCI_IPC_VOID, values)
    CHECK_VECTOR_ENABLED(vect->con)

#if OCI_VERSION_COMPILE >= OCI_23_4

    CHECK_ENUM_VALUE(format, VectorFormatValues, OTEXT("Vector format"))

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
    )

    SET_SUCCESS()

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorFromString
 * --------------------------------------------------------------------------------------------- */

OCI_SYM_LOCAL boolean OcilibVectorFromString
(
    OCI_Vector  *vect,
    const otext* str,
    unsigned int size,
    unsigned int format,
    unsigned int dimensions
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    )

    char *ansi_str  = NULL;

    CHECK_PTR(OCI_IPC_VECTOR, vect)
    CHECK_PTR(OCI_IPC_VOID, str)
    CHECK_VECTOR_ENABLED(vect->con)

    ub4 ansi_size = (ub4) (size == 0 ? ostrlen(str)  : size);

#if OCI_VERSION_COMPILE >= OCI_23_4

    CHECK_ENUM_VALUE(format, VectorFormatValues, OTEXT("Vector format"))

    if (IS_WIDE_CHAR_ENV())
    {
        ansi_str = OcilibMemoryAlloc(OCI_IPC_STRING, (size_t)ansi_size+1, (size_t)1, TRUE);
        OcilibStringNativeToAnsi(str, ansi_str, ansi_size);
    }
    else
    {
        ansi_str = (char*) str;
    }
             
    CHECK_OCI
    (
        vect->con->err,
        OCIVectorFromText,
        vect->handle,
        vect->con->err,
        (ub1) format, 
        (ub4) dimensions,
        (const OraText*) ansi_str,
        (ub4) ansi_size,
        OCI_DEFAULT
    )

     SET_SUCCESS()

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (IS_WIDE_CHAR_ENV())
        {    
            OcilibMemoryFree(ansi_str);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibVectorToString
 * --------------------------------------------------------------------------------------------- */

OCI_SYM_LOCAL boolean OcilibVectorToString
(
    OCI_Vector* vect,
    unsigned int* size,
    otext* str

)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VECTOR, vect
    )
  
    char *ansi_str  = NULL;

    CHECK_PTR(OCI_IPC_VECTOR, vect)
    CHECK_PTR(OCI_IPC_VOID, size)
    CHECK_VECTOR_ENABLED(vect->con)

    ub4 ansi_size = (ub4) *size;

#if OCI_VERSION_COMPILE >= OCI_23_4

    if (str == NULL)
    {
        unsigned int dimensions = 0;
        unsigned int format     = OCI_VEC_FLEX;

        CHECK(OcilibVectorGetInfo(vect, &format, &dimensions))

        unsigned int elem_size = (format == OCI_VEC_FLOAT32 || format == OCI_VEC_FLOAT64) ?
            OCI_SIZE_NUM_VECTOR_FLOAT : OCI_SIZE_NUM_VECTOR_INT;

        ansi_size = (ub4) (2 + (elem_size + 2) * dimensions);
    }
    else
    {
        str[0] = 0;

        if (IS_WIDE_CHAR_ENV())
        {
            ansi_str = OcilibMemoryAlloc(OCI_IPC_STRING, (size_t)ansi_size+1, (size_t)1, TRUE);
        }
        else
        {
            ansi_str = (char*) str;
        }

        CHECK_OCI
        (
            vect->con->err,
            OCIVectorToText,
            vect->handle,
            vect->con->err,
            (OraText*)ansi_str,
            &ansi_size,
            OCI_DEFAULT
        )

        if (IS_WIDE_CHAR_ENV())
        {    
            OcilibStringAnsiToNative(ansi_str ,str, ansi_size);
        }
        else
        {
            str[ansi_size] = 0;
        }
    }

    *size = (unsigned int)ansi_size;

#endif

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        if (IS_WIDE_CHAR_ENV())
        {    
            OcilibMemoryFree(ansi_str);
        }
    )
}
