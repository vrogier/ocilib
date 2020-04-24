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
    DECLARE_CTX(TRUE)
    CTX_SET_FROM_CON(con)

    ALLOC_DATA(OCI_IPC_REF, ref, 1);

    if (STATUS)
    {
        ref->handle = handle;
        ref->con    = con;
        ref->typinf = typinf;

        if (!ref->handle || (OCI_OBJECT_ALLOCATED_ARRAY == ref->hstate))
        {
            /* allocates handle for non fetched object */

            if (OCI_OBJECT_ALLOCATED_ARRAY != ref->hstate)
            {
                ref->hstate = OCI_OBJECT_ALLOCATED;
            }

            EXEC
            (
                MemoryAllocateObject(ref->con->env, ref->con->err, ref->con->cxt,
                          (OCITypeCode) SQLT_REF,
                          (OCIType*) NULL,
                          (dvoid *) NULL,
                          (OCIDuration) OCI_DURATION_SESSION,
                          (boolean) FALSE,
                          (dvoid **) &ref->handle)
            )
        }
        else
        {
            ref->hstate = OCI_OBJECT_FETCHED_CLEAN;

            ReferenceUnpin(ref);
        }
    }

    /* check for failure */

    if (!STATUS && ref)
    {
        ReferenceFree(ref);
        ref = NULL;
    }

    return ref;
}

/* --------------------------------------------------------------------------------------------- *
 * ReferencePin
 * --------------------------------------------------------------------------------------------- */

boolean ReferencePin
(
    OCI_Ref *ref
)
{
    void *obj_handle = NULL;

    DECLARE_CTX(TRUE)

    CHECK(NULL == ref, FALSE)

    CTX_SET_FROM_CON(ref->con);

    ReferenceUnpin(ref);

    EXEC
    (
        OCIObjectPin(ref->con->env, ref->con->err, ref->handle, (OCIComplexObject *) 0, 
                     OCI_PIN_ANY, OCI_DURATION_SESSION, OCI_LOCK_NONE, &obj_handle)
    )

    if (STATUS)
    {
        ref->obj = ObjectInitialize(ref->con, (OCI_Object *) ref->obj, obj_handle, ref->typinf, NULL, -1, TRUE);

        STATUS = ref->pinned = (NULL != ref->obj);
    }

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceUnpin
 * --------------------------------------------------------------------------------------------- */

boolean ReferenceUnpin
(
    OCI_Ref *ref
)
{
    DECLARE_CTX(TRUE)

    CHECK(NULL == ref, FALSE)
    CHECK(NULL == ref->obj, TRUE)

    CTX_SET_FROM_CON(ref->con);

    if (ref->pinned)
    {
        EXEC(OCIObjectUnpin(ref->con->env, ref->con->err, ref->obj->handle))

        ref->pinned = FALSE;
    }

    if (ref->obj)
    {
        ref->obj->hstate = OCI_OBJECT_FETCHED_DIRTY;
        ObjectFree(ref->obj);
        ref->obj = NULL;
    }

    return STATUS;
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
    CALL_ENTER(OCI_Ref *, NULL)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    CTX_SET_FROM_CON(con)

    RETVAL = ReferenceInitialize(con, typinf, NULL, NULL);
    STATUS = (NULL != RETVAL);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceFree
 * --------------------------------------------------------------------------------------------- */

boolean ReferenceFree
(
    OCI_Ref *ref
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_REF, ref)
    CHECK_OBJECT_FETCHED(ref)
    CTX_SET_FROM_CON(ref->con)

    ReferenceUnpin(ref);

    if ((OCI_OBJECT_ALLOCATED == ref->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == ref->hstate))
    {
        MemoryFreeObject(ref->con->env, ref->con->err, ref->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != ref->hstate)
    {
        FREE(ref)
    }

    RETVAL = STATUS;

    CALL_EXIT()
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
    OCI_Array *arr = NULL;

    CALL_ENTER(OCI_Ref **, NULL)
    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_PTR(OCI_IPC_TYPE_INFO, con)
    CTX_SET_FROM_CON(con)

    arr = ArrayCreate(con, nbelem, OCI_CDT_REF, 0, sizeof(OCIRef *), sizeof(OCI_Ref), 0, typinf);
    STATUS = (NULL != arr);

    if (STATUS)
    {
        RETVAL = (OCI_Ref **) arr->tab_obj;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceFreeArray
 * --------------------------------------------------------------------------------------------- */

boolean ReferenceFreeArray
(
    OCI_Ref **refs
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_ARRAY, refs)

    RETVAL = STATUS = ArrayFreeFromHandles((void **)refs);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * ReferenceGetObject
(
    OCI_Ref *ref
)
{
    CALL_ENTER(OCI_Object*, NULL)
    CHECK_PTR(OCI_IPC_REF, ref)
    CTX_SET_FROM_CON(ref->con)

    if (!ReferenceIsNull(ref))
    {
        STATUS = ReferencePin(ref);

        if (STATUS)
        {
            RETVAL = ref->obj;
        }
    }

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_REF, ref)
    CHECK_PTR(OCI_IPC_REF, ref_src)
    CHECK_COMPAT(ref->con, ref->typinf->tdo == ref_src->typinf->tdo)
    CTX_SET_FROM_CON(ref->con)

    EXEC(OCIRefAssign(ref->con->env, ref->con->err, ref_src->handle, &ref->handle))

    if (STATUS)
    {
        if (ref->obj)
        {
            ObjectFree(ref->obj);
            ref->obj = NULL;
        }

        ref->typinf = ref_src->typinf;
        ref->pinned = ref_src->pinned;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceIsNull
 * --------------------------------------------------------------------------------------------- */

boolean ReferenceIsNull
(
    OCI_Ref *ref
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_REF, ref)
    CTX_SET_FROM_CON(ref->con)

    RETVAL = OCIRefIsNull(ref->con->env, ref->handle);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceSetNull
 * --------------------------------------------------------------------------------------------- */

boolean ReferenceSetNull
(
    OCI_Ref *ref
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_REF, ref)
    CTX_SET_FROM_CON(ref->con)

    STATUS = ReferenceUnpin(ref);

    if (STATUS)
    {
        OCIRefClear(ref->con->env, ref->handle);

        if (ref->obj)
        {
            ObjectFree(ref->obj);
            ref->obj = NULL;
        }
    }

    RETVAL = STATUS;

    CALL_EXIT()
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
    dbtext *dbstr  = NULL;
    int     dbsize = (int) size * (int) sizeof(otext);

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_REF, ref)
    CHECK_PTR(OCI_IPC_STRING, str)
    CTX_SET_FROM_CON(ref->con)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    dbstr = StringGetDBString(str, &dbsize);

    EXEC(OCIRefToHex(ref->con->env, ref->con->err, ref->handle, (OraText *) dbstr, (ub4 *) &dbsize))

    StringCopyDBStringToNativeString(dbstr, str, dbcharcount(dbsize));
    StringReleaseDBString(dbstr);

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ReferenceGetHexSize
 * --------------------------------------------------------------------------------------------- */

unsigned int ReferenceGetHexSize
(
    OCI_Ref *ref
)
{
    ub4 size = 0;

    CALL_ENTER(unsigned int, 0)
    CHECK_PTR(OCI_IPC_REF, ref)
    CTX_SET_FROM_CON(ref->con)

    size = OCIRefHexSize(ref->con->env, (const OCIRef *)ref->handle) / (ub4) sizeof(dbtext);

    RETVAL = size;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollRefGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * ReferenceGetTypeInfo
(
    OCI_Ref *ref
)
{
    GET_PROP(OCI_TypeInfo*, NULL, OCI_IPC_REF, ref, typinf, ref->con, NULL, ref->con->err)
}

