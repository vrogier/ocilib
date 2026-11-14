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

#include "reference.h"

#include "array.h"
#include "macros.h"
#include "memory.h"
#include "object.h"
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Reference * OcilibReferenceInitialize
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    OCI_Reference  *ref,
    void           *handle
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Reference*, ref,
        /* context */ OCI_IPC_CONNECTION, con
    );

    ALLOC_HANDLE(OCI_IPC_REF, ref);
    ref->handle = handle;
    ref->con    = con;
    ref->typinf = typinf;

    if (NULL == ref->handle || (OCI_OBJECT_ALLOCATED_ARRAY == ref->hstate))
    {
        /* allocates handle for non fetched object */

        if (OCI_OBJECT_ALLOCATED_ARRAY != ref->hstate)
        {
            ref->hstate = OCI_OBJECT_ALLOCATED;
        }

        CHECK(OcilibMemoryAllocateObject(ref->con->env,
                                         ref->con->env->handle,
                                         ref->con->err,
                                         ref->con->handle,
                                         (OCITypeCode) SQLT_REF,
                                         (OCIType*) NULL,
                                         (dvoid *) NULL,
                                         (OCIDuration) OCI_DURATION_SESSION,
                                         (OCI_Boolean) FALSE,
                                         (dvoid **) &ref->handle));
    }
    else
    {
        ref->hstate = OCI_OBJECT_FETCHED_CLEAN;

        CHECK(OcilibReferenceUnpin(ref));
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibReferenceFree(ref);
            ref = NULL;
        }

        SET_RETVAL(ref);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferencePin
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibReferencePin
(
    OCI_Reference *ref
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    );

    void *obj_handle = NULL;

    CHECK_PTR(OCI_IPC_REF, ref);

    CHECK(OcilibReferenceUnpin(ref));

    CHECK_OCI
    (
        ref->con->err,
        OCIObjectPin,
        ref->con->env->handle, ref->con->err, ref->handle, (OCIComplexObject *) 0,
        OCI_PIN_ANY, OCI_DURATION_SESSION, OCI_LOCK_NONE, &obj_handle
    );

    ref->obj = OcilibObjectInitialize(ref->con, (OCI_Object *) ref->obj,
                                      obj_handle, ref->typinf, NULL,
                                      -1, TRUE);

    CHECK_NULL(ref->obj);

    ref->pinned = TRUE;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceUnpin
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibReferenceUnpin
(
    OCI_Reference *ref
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    );

    CHECK_PTR(OCI_IPC_REF, ref);

    if (NULL != ref->obj)
    {
        if (ref->pinned)
        {
            CHECK_OCI
            (
                ref->con->err,
                OCIObjectUnpin,
                ref->con->env->handle, ref->con->err,
                ref->obj->handle
            );

            ref->pinned = FALSE;
        }

        ref->obj->hstate = OCI_OBJECT_FETCHED_DIRTY;
        OcilibObjectFree(ref->obj);
        ref->obj = NULL;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Reference * OcilibReferenceCreate
(
    OCI_TypeInfo *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Reference*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    CHECK_PTR(OCI_IPC_TYPE_INFO,  typinf);

    SET_RETVAL(OcilibReferenceInitialize(typinf->con, typinf, NULL, NULL));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibReferenceFree
(
    OCI_Reference *ref
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    );

    CHECK_PTR(OCI_IPC_REF, ref);
    CHECK_OBJECT_FETCHED(ref);

    CHECK(OcilibReferenceUnpin(ref));

    if ((OCI_OBJECT_ALLOCATED == ref->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == ref->hstate))
    {
        OcilibMemoryFreeObject(ref->con->env, ref->con->env->handle, ref->con->err, ref->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != ref->hstate)
    {
        OcilibErrorResetSource(NULL, ref);

        FREE(ref);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Reference ** OcilibReferenceArrayCreate
(
    OCI_TypeInfo *typinf,
    OCI_UInt32    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Reference**, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_TYPE_INFO,  typinf);

    arr = OcilibArrayCreate(typinf->con->env,typinf->con, nbelem, OCI_CDT_REF, 0,
                            sizeof(OCIRef *), sizeof(OCI_Reference),
                            0, typinf);

    CHECK_NULL(arr);

    SET_RETVAL((OCI_Reference**)arr->tab_obj);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceArrayFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibReferenceArrayFree
(
    OCI_Reference **refs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, NULL
    );

    CHECK_PTR(OCI_IPC_ARRAY, refs);

    SET_RETVAL(OcilibArrayFreeFromHandles(((OCI_Reference *) refs[0])->con->env, (void **)refs));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OcilibReferenceGetObject
(
    OCI_Reference *ref
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Object*, NULL,
        /* context */ OCI_IPC_REF, ref
    );

    CHECK_PTR(OCI_IPC_REF, ref);
    CHECK(!OcilibReferenceIsNull(ref));
    CHECK(OcilibReferencePin(ref));

    SET_RETVAL(ref->obj);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibReferenceAssign
(
    OCI_Reference *ref,
    OCI_Reference *ref_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    );

    CHECK_PTR(OCI_IPC_REF, ref);
    CHECK_PTR(OCI_IPC_REF, ref_src);
    CHECK_COMPAT(ref->typinf->handle == ref_src->typinf->handle);

    CHECK_OCI
    (
        ref->con->err,
        OCIRefAssign,
        ref->con->env->handle, ref->con->err,
        ref_src->handle, &ref->handle
    );

    if (NULL != ref->obj)
    {
        OcilibObjectFree(ref->obj);
        ref->obj = NULL;
    }

    ref->typinf = ref_src->typinf;
    ref->pinned = ref_src->pinned;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceIsNull
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibReferenceIsNull
(
    OCI_Reference *ref
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    );

    CHECK_PTR(OCI_IPC_REF, ref);

    OCI_Boolean is_null;

    OCI_CALL(is_null, OCIRefIsNull, ref->con->env->handle, ref->handle);

    SET_RETVAL(is_null);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceSetNull
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibReferenceSetNull
(
    OCI_Reference *ref
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    );

    CHECK_PTR(OCI_IPC_REF, ref);

    CHECK(OcilibReferenceUnpin(ref));

    ref->con->env->api->OCIRefClear(ref->con->env->handle, ref->handle);

    if (NULL != ref->obj)
    {
        OcilibObjectFree(ref->obj);
        ref->obj = NULL;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceToText
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibReferenceToText
(
    OCI_Reference *ref,
    OCI_UInt32     size,
    otext         *str
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    );

    dbtext *dbstr = NULL;

    /* OCI hex string are always MBCS strings whatever environment mode */
    OCI_UInt32 char_count = size;

    CHECK_PTR(OCI_IPC_REF,    ref);
    CHECK_PTR(OCI_IPC_STRING, str);

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    if (IS_WIDE_ENV())
    {
        /* need temporary buffer in wide strings mode */
        dbstr = (dbtext *) OcilibMemoryAlloc(ref->con->env,OCI_IPC_STRING, (OCI_Size) ((char_count + 1) * sizeof(otext)), (OCI_Size) 1,
                                             TRUE);
    }
    else
    {
        dbstr = (dbtext *) str;
    }

    CHECK_OCI
    (
        ref->con->err,
        OCIRefToHex,
        ref->con->env->handle, ref->con->err, ref->handle,
        (OraText *) dbstr, (ub4 *) &char_count
    );

    if (IS_WIDE_ENV())
    {
        /* need to convert temporary MBCS buffer to Native in wide strings mode */
        OcilibStringMbcsToNative((const char *) dbstr, str, (OCI_Size) char_count);
    }

    /* set null string terminator */

    str[char_count] = 0;

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (IS_WIDE_ENV())
        {
            FREE(dbstr);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceGetHexSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibReferenceGetHexSize
(
    OCI_Reference *ref
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_REF, ref
    );

    CHECK_PTR(OCI_IPC_REF, ref);

    OCI_UInt32 hexsize;

    OCI_CALL(hexsize, OCIRefHexSize, ref->con->env->handle, (const OCIRef*)ref->handle);

    SET_RETVAL(hexsize);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibReferenceGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * OcilibReferenceGetTypeInfo
(
    OCI_Reference *ref
)
{
    GET_PROP
    (
        OCI_TypeInfo*, NULL,
        OCI_IPC_REF, ref,
        typinf
    );
}
