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

#include "reference.h"

#include "array.h"
#include "macros.h"
#include "memory.h"
#include "object.h"
#include "strings.h"

/* --------------------------------------------------------------------------------------------- *
 * ReferenceInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * ReferenceInitialize
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    OCI_Ref        *ref,
    void           *handle
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Ref*, ref,
        /* context */ OCI_IPC_CONNECTION, con
    )

    ALLOC_DATA(OCI_IPC_REF, ref, 1);

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

        CHECK(MemoryAllocateObject(ref->con->env,
                                   ref->con->err,
                                   ref->con->cxt,
                                   (OCITypeCode) SQLT_REF,
                                   (OCIType*) NULL,
                                   (dvoid *) NULL,
                                   (OCIDuration) OCI_DURATION_SESSION,
                                   (boolean) FALSE,
                                   (dvoid **) &ref->handle))
    }
    else
    {
        ref->hstate = OCI_OBJECT_FETCHED_CLEAN;

        CHECK(ReferenceUnpin(ref))
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            ReferenceFree(ref);
            ref = NULL;
        }

        SET_RETVAL(ref)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ReferencePin
 * --------------------------------------------------------------------------------------------- */

boolean ReferencePin
(
    OCI_Ref *ref
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    )

    void *obj_handle = NULL;

    CHECK_PTR(OCI_IPC_REF, ref)

    CHECK(ReferenceUnpin(ref))

    CHECK_OCI
    (
        ref->con->err,
        OCIObjectPin,
        ref->con->env, ref->con->err, ref->handle, (OCIComplexObject *) 0,
        OCI_PIN_ANY, OCI_DURATION_SESSION, OCI_LOCK_NONE, &obj_handle
    )

    ref->obj =  ObjectInitialize(ref->con, (OCI_Object *) ref->obj,
                                 obj_handle, ref->typinf, NULL,
                                 -1, TRUE);

    CHECK_NULL(ref->obj)

    ref->pinned = TRUE;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceUnpin
 * --------------------------------------------------------------------------------------------- */

boolean ReferenceUnpin
(
    OCI_Ref *ref
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    )

    CHECK_PTR(OCI_IPC_REF, ref)

    if (NULL != ref->obj)
    {
        if (ref->pinned)
        {
            CHECK_OCI
            (
                ref->con->err,
                OCIObjectUnpin,
                ref->con->env, ref->con->err,
                ref->obj->handle
            )

            ref->pinned = FALSE;
        }

        ref->obj->hstate = OCI_OBJECT_FETCHED_DIRTY;
        ObjectFree(ref->obj);
        ref->obj = NULL;
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * ReferenceCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Ref*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_TYPE_INFO,  typinf)

    SET_RETVAL(ReferenceInitialize(con, typinf, NULL, NULL))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceFree
 * --------------------------------------------------------------------------------------------- */

boolean ReferenceFree
(
    OCI_Ref *ref
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    )

    CHECK_PTR(OCI_IPC_REF, ref)
    CHECK_OBJECT_FETCHED(ref)

    CHECK(ReferenceUnpin(ref))

    if ((OCI_OBJECT_ALLOCATED == ref->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == ref->hstate))
    {
        MemoryFreeObject(ref->con->env, ref->con->err, ref->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != ref->hstate)
    {
        FREE(ref)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Ref ** ReferenceCreateArray
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Ref**, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_TYPE_INFO,  con)

    arr = ArrayCreate(con, nbelem, OCI_CDT_REF, 0,
                      sizeof(OCIRef *), sizeof(OCI_Ref),
                      0, typinf);

    CHECK_NULL(arr)

    SET_RETVAL((OCI_Ref**)arr->tab_obj)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceFreeArray
 * --------------------------------------------------------------------------------------------- */

boolean ReferenceFreeArray
(
    OCI_Ref **refs
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_PTR(OCI_IPC_ARRAY, refs)

    SET_RETVAL(ArrayFreeFromHandles((void **)refs))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * ReferenceGetObject
(
    OCI_Ref *ref
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Object*, NULL,
        /* context */ OCI_IPC_REF, ref
    )

    CHECK_PTR(OCI_IPC_REF, ref)
    CHECK(!ReferenceIsNull(ref))
    CHECK(ReferencePin(ref))

    SET_RETVAL(ref->obj)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceAssign
 * --------------------------------------------------------------------------------------------- */

boolean ReferenceAssign
(
    OCI_Ref *ref,
    OCI_Ref *ref_src
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    )

    CHECK_PTR(OCI_IPC_REF, ref)
    CHECK_PTR(OCI_IPC_REF, ref_src)
    CHECK_COMPAT(ref->typinf->tdo == ref_src->typinf->tdo)

    CHECK_OCI
    (
        ref->con->err,
        OCIRefAssign,
        ref->con->env, ref->con->err,
        ref_src->handle, &ref->handle
    )

    if (NULL != ref->obj)
    {
        ObjectFree(ref->obj);
        ref->obj = NULL;
    }

    ref->typinf = ref_src->typinf;
    ref->pinned = ref_src->pinned;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceIsNull
 * --------------------------------------------------------------------------------------------- */

boolean ReferenceIsNull
(
    OCI_Ref *ref
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    )

    CHECK_PTR(OCI_IPC_REF, ref)

    SET_RETVAL(OCIRefIsNull(ref->con->env, ref->handle))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceSetNull
 * --------------------------------------------------------------------------------------------- */

boolean ReferenceSetNull
(
    OCI_Ref *ref
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    )

    CHECK_PTR(OCI_IPC_REF, ref)

    CHECK(ReferenceUnpin(ref))

    OCIRefClear(ref->con->env, ref->handle);

    if (NULL != ref->obj)
    {
        ObjectFree(ref->obj);
        ref->obj = NULL;
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceToString
 * --------------------------------------------------------------------------------------------- */

boolean ReferenceToString
(
    OCI_Ref     *ref,
    unsigned int size,
    otext       *str
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_REF, ref
    )

    dbtext *dbstr = NULL;
    int dbsize = (int) size * (int) sizeof(otext);

    CHECK_PTR(OCI_IPC_REF,    ref)
    CHECK_PTR(OCI_IPC_STRING, str)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    dbstr = StringGetDBString(str, &dbsize);

    CHECK_OCI
    (
        ref->con->err,
        OCIRefToHex,
        ref->con->env, ref->con->err, ref->handle,
        (OraText *) dbstr, (ub4 *) &dbsize
    )

    StringCopyDBStringToNativeString(dbstr, str, dbcharcount(dbsize));

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceGetHexSize
 * --------------------------------------------------------------------------------------------- */

unsigned int ReferenceGetHexSize
(
    OCI_Ref *ref
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_REF, ref
    )

    CHECK_PTR(OCI_IPC_REF, ref)

    SET_RETVAL((unsigned int) OCIRefHexSize(ref->con->env, (const OCIRef*)ref->handle) / (ub4)sizeof(dbtext))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollRefGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * ReferenceGetTypeInfo
(
    OCI_Ref *ref
)
{
    GET_PROP
    (
        OCI_TypeInfo*, NULL,
        OCI_IPC_REF, ref,
        typinf
    )
}
