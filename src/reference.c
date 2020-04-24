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
#include "macro.h"
#include "memory.h"
#include "object.h"
#include "strings.h"

/* --------------------------------------------------------------------------------------------- *
 * RefInit
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * RefInit
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    OCI_Ref        *ref,
    void           *handle
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_ALLOCATE_DATA(OCI_IPC_REF, ref, 1);

    if (OCI_STATUS)
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

            OCI_EXEC
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

            RefUnpin(ref);
        }
    }

    /* check for failure */

    if (!OCI_STATUS && ref)
    {
        RefFree(ref);
        ref = NULL;
    }

    return ref;
}

/* --------------------------------------------------------------------------------------------- *
 * RefPin
 * --------------------------------------------------------------------------------------------- */

boolean RefPin
(
    OCI_Ref *ref
)
{
    void *obj_handle = NULL;

    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == ref, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con);

    RefUnpin(ref);

    OCI_EXEC
    (
        OCIObjectPin(ref->con->env, ref->con->err, ref->handle, (OCIComplexObject *) 0, 
                     OCI_PIN_ANY, OCI_DURATION_SESSION, OCI_LOCK_NONE, &obj_handle)
    )

    if (OCI_STATUS)
    {
        ref->obj = ObjectInit(ref->con, (OCI_Object *) ref->obj, obj_handle, ref->typinf, NULL, -1, TRUE);

        OCI_STATUS = ref->pinned = (NULL != ref->obj);
    }

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * RefUnpin
 * --------------------------------------------------------------------------------------------- */

boolean RefUnpin
(
    OCI_Ref *ref
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == ref, FALSE)
    OCI_CHECK(NULL == ref->obj, TRUE)

    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con);

    if (ref->pinned)
    {
        OCI_EXEC(OCIObjectUnpin(ref->con->env, ref->con->err, ref->obj->handle))

        ref->pinned = FALSE;
    }

    if (ref->obj)
    {
        ref->obj->hstate = OCI_OBJECT_FETCHED_DIRTY;
        ObjectFree(ref->obj);
        ref->obj = NULL;
    }

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * RefCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * RefCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf
)
{
    OCI_CALL_ENTER(OCI_Ref *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_RETVAL = RefInit(con, typinf, NULL, NULL);
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * RefFree
 * --------------------------------------------------------------------------------------------- */

boolean RefFree
(
    OCI_Ref *ref
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CHECK_OBJECT_FETCHED(ref)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    RefUnpin(ref);

    if ((OCI_OBJECT_ALLOCATED == ref->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == ref->hstate))
    {
        MemoryFreeObject(ref->con->env, ref->con->err, ref->handle, OCI_DEFAULT);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != ref->hstate)
    {
        OCI_FREE(ref)
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * RefArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Ref ** RefArrayCreate
(
    OCI_Connection *con,
    OCI_TypeInfo   *typinf,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    OCI_CALL_ENTER(OCI_Ref **, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    arr = ArrayCreate(con, nbelem, OCI_CDT_REF, 0, sizeof(OCIRef *), sizeof(OCI_Ref), 0, typinf);
    OCI_STATUS = (NULL != arr);

    if (OCI_STATUS)
    {
        OCI_RETVAL = (OCI_Ref **) arr->tab_obj;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * RefArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean RefArrayFree
(
    OCI_Ref **refs
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ARRAY, refs)

    OCI_RETVAL = OCI_STATUS = ArrayFreeFromHandles((void **)refs);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * RefGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * RefGetObject
(
    OCI_Ref *ref
)
{
    OCI_CALL_ENTER(OCI_Object*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    if (!RefIsNull(ref))
    {
        OCI_STATUS = RefPin(ref);

        if (OCI_STATUS)
        {
            OCI_RETVAL = ref->obj;
        }
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * RefAssign
 * --------------------------------------------------------------------------------------------- */

boolean RefAssign
(
    OCI_Ref *ref,
    OCI_Ref *ref_src
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref_src)
    OCI_CALL_CHECK_COMPAT(ref->con, ref->typinf->tdo == ref_src->typinf->tdo)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    OCI_EXEC(OCIRefAssign(ref->con->env, ref->con->err, ref_src->handle, &ref->handle))

    if (OCI_STATUS)
    {
        if (ref->obj)
        {
            ObjectFree(ref->obj);
            ref->obj = NULL;
        }

        ref->typinf = ref_src->typinf;
        ref->pinned = ref_src->pinned;
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * RefIsNull
 * --------------------------------------------------------------------------------------------- */

boolean RefIsNull
(
    OCI_Ref *ref
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    OCI_RETVAL = OCIRefIsNull(ref->con->env, ref->handle);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * RefSetNull
 * --------------------------------------------------------------------------------------------- */

boolean RefSetNull
(
    OCI_Ref *ref
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    OCI_STATUS = RefUnpin(ref);

    if (OCI_STATUS)
    {
        OCIRefClear(ref->con->env, ref->handle);

        if (ref->obj)
        {
            ObjectFree(ref->obj);
            ref->obj = NULL;
        }
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * RefToText
 * --------------------------------------------------------------------------------------------- */

boolean RefToText
(
    OCI_Ref     *ref,
    unsigned int size,
    otext       *str
)
{
    dbtext *dbstr  = NULL;
    int     dbsize = (int) size * (int) sizeof(otext);

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, str)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    dbstr = StringGetOracleString(str, &dbsize);

    OCI_EXEC(OCIRefToHex(ref->con->env, ref->con->err, ref->handle, (OraText *) dbstr, (ub4 *) &dbsize))

    StringCopyOracleStringToNativeString(dbstr, str, dbcharcount(dbsize));
    StringReleaseOracleString(dbstr);

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * RefGetHexSize
 * --------------------------------------------------------------------------------------------- */

unsigned int RefGetHexSize
(
    OCI_Ref *ref
)
{
    ub4 size = 0;

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_REF, ref)
    OCI_CALL_CONTEXT_SET_FROM_CONN(ref->con)

    size = OCIRefHexSize(ref->con->env, (const OCIRef *)ref->handle) / (ub4) sizeof(dbtext);

    OCI_RETVAL = size;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_CollRefGetTypeInfo
 * --------------------------------------------------------------------------------------------- */

OCI_TypeInfo * RefGetTypeInfo
(
    OCI_Ref *ref
)
{
    OCI_GET_PROP(OCI_TypeInfo*, NULL, OCI_IPC_REF, ref, typinf, ref->con, NULL, ref->con->err)
}

